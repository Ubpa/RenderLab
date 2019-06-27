#include <CppUtil/Engine/PathTracer.h>

#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/VisibilityChecker.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptMaterial.h>
#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/Light.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

PathTracer::PathTracer()
	:
	maxDepth(20),
	rayIntersector(RayIntersector::New()),
	visibilityChecker(VisibilityChecker::New())
{ }

void PathTracer::Init(Ptr<Scene> scene, Ptr<BVHAccel> bvhAccel) {
	RayTracer::Init(scene, bvhAccel);

	lights.clear();
	worldToLightVec.clear();
	lightToWorldVec.clear();
	lightToIdx.clear();

	auto cmptLights = scene->GetCmptLights();
	for (size_t i = 0; i < cmptLights.size(); i++) {
		auto cmptLight = cmptLights[i];
		auto light = cmptLight->light;

		lightToIdx[light] = static_cast<int>(i);

		lights.push_back(light);

		const auto lightToWorld = cmptLight->GetLightToWorldMatrixWithoutScale();
		const auto worldToLight = lightToWorld.Inverse();

		worldToLightVec.push_back(worldToLight);
		lightToWorldVec.push_back(lightToWorld);
	}
}

const RGBf PathTracer::Trace(ERay & ray, int depth, RGBf pathThroughput) {
	rayIntersector->Init(&ray);
	bvhAccel->Accept(rayIntersector);
	auto closestRst = rayIntersector->GetRst();
	if (!closestRst.closestSObj) {
		RGBf Le(0.f);
		for (auto light : lights)
			Le += light->Le(ray);

		return Le;
	}

	const Point3 hitPos = ray.EndPos();

	auto cmptMaterial = closestRst.closestSObj->GetComponent<CmptMaterial>();
	if (!cmptMaterial || !cmptMaterial->material)
		return RGBf(0);

	auto bsdf = dynamic_pointer_cast<BSDF>(cmptMaterial->material);
	if (bsdf == nullptr)
		return Vec3f(0);

	bsdf->ChangeNormal(closestRst.texcoord, closestRst.tangent, closestRst.n);

	const auto surfaceToWorld = closestRst.n.GenCoordSpace();
	const auto worldToSurface = surfaceToWorld.Transpose();

	// w_out 处于表面坐标系，向外
	const Normalf w_out = (worldToSurface * (-ray.d)).Normalize();

	RGBf emitL = depth == 0 ? bsdf->Emission(w_out) : RGBf(0);

	// SampleLightMode mode = depth > 0 ? SampleLightMode::RandomOne : SampleLightMode::ALL;
	SampleLightMode mode = SampleLightMode::RandomOne;
	const RGBf lightL = SampleLight(hitPos, worldToSurface, bsdf, w_out, closestRst.texcoord, SampleLightMode::RandomOne);

	const RGBf matL = SampleBSDF(bsdf, mode, w_out, surfaceToWorld, closestRst.texcoord, hitPos, depth, pathThroughput);

	return emitL + lightL + matL;
}

const RGBf PathTracer::SampleLightImpl(
	const int lightID,
	const Point3 & posInWorldSpace,
	const Point3 & posInLightSpace,
	const Mat3f & worldToSurface,
	const Basic::Ptr<BSDF> bsdf,
	const Normalf & w_out,
	const Point2 & texcoord,
	float factorPD
) const
{
	auto const light = lights[lightID];
	auto const & lightToWorld = lightToWorldVec[lightID];
	const int lightNum = static_cast<int>(lights.size());

	float dist_ToLight;
	float PD;// 概率密度
	Normalf dir_ToLight;
	// dir_ToLight 是单位向量
	const RGBf lightL = light->Sample_L(posInLightSpace, dir_ToLight, dist_ToLight, PD);
	PD *= factorPD;
	if (PD == 0)
		return RGBf(0.f);

	const Normalf dirInWorld = lightToWorld(dir_ToLight).Normalize();

	// w_in 处于表面坐标系，应该是单位向量
	const Normalf w_in = (worldToSurface * dirInWorld).Normalize();

	// evaluate surface bsdf
	const RGBf f = bsdf->F(w_out, w_in, texcoord);
	if (f.IsZero())
		return RGBf(0.f);

	// shadow ray 处于世界坐标
	ERay shadowRay(posInWorldSpace, dirInWorld);
	visibilityChecker->Init(shadowRay, dist_ToLight - 0.001f);
	bvhAccel->Accept(visibilityChecker);
	auto shadowRst = visibilityChecker->GetRst();
	if (shadowRst.IsIntersect())
		return RGBf(0);

	// 多重重要性采样 Multiple Importance Sampling (MIS)
	if (!light->IsDelta()) {
		for (int k = 0; k < lightNum; k++) {
			if (k != lightID && !lights[k]->IsDelta()) {
				const auto dirInLight = worldToLightVec[k](dirInWorld).Normalize();
				PD += lights[k]->PDF(posInLightSpace, dirInLight) * factorPD;
			}
		}
		PD += bsdf->PDF(w_out, w_in, texcoord);
	}

	auto weight = (abs(w_in.z) / PD) * f;
	return weight * lightL;
}

const RGBf PathTracer::SampleLight(
	const Point3 & posInWorldSpace,
	const Mat3f & worldToSurface,
	const Basic::Ptr<BSDF> bsdf,
	const Normalf & w_out,
	const Point2 & texcoord,
	const SampleLightMode mode
) const
{
	if (bsdf->IsDelta())
		return RGBf(0.f);

	int lightNum = static_cast<int>(lights.size());
	RGBf rst(0.f);

	switch (mode)
	{
	case SampleLightMode::ALL: {
		for (int i = 0; i < lightNum; i++) {
			auto posInLightSpace = worldToLightVec[i](posInWorldSpace);
			rst += SampleLightImpl(i, posInWorldSpace, posInLightSpace, worldToSurface, bsdf, w_out, texcoord, 1.f);
		}

		break;
	}
	case SampleLightMode::RandomOne: {
		int lightID = Math::Rand_I() % lightNum;
		auto posInLightSpace = worldToLightVec[lightID](posInWorldSpace);
		rst = SampleLightImpl(lightID, posInWorldSpace, posInLightSpace, worldToSurface, bsdf, w_out, texcoord, 1.f / lightNum);
		break;
	}
	}

	return rst;
}

const RGBf PathTracer::SampleBSDF(
	const Basic::Ptr<BSDF> bsdf,
	const SampleLightMode mode,
	const Normalf & w_out,
	const Mat3f & surfaceToWorld,
	const Point2 & texcoord,
	const Point3 & hitPos,
	const int depth,
	RGBf pathThroughput
)
{
	if (depth + 1 >= maxDepth)
		return RGBf(0.f);

	Normalf mat_w_in;
	float matPD;
	const RGBf matF = bsdf->Sample_f(w_out, texcoord, mat_w_in, matPD);
	if (matPD <= 0)
		return RGBf(0);

	const Normalf matRayDirInWorld = (surfaceToWorld * mat_w_in).Normalize();
	const int lightNum = static_cast<int>(lights.size());

	// MSI
	float sumPD = matPD;
	if (!bsdf->IsDelta()) {
		float scaleFactor;
		switch (mode)
		{
		case SampleLightMode::ALL:
			scaleFactor = 1.f;
			break;
		case SampleLightMode::RandomOne:
			scaleFactor = 1.f / lightNum;
			break;
		}

		for (int i = 0; i < lightNum; i++) {
			if (lights[i]->IsDelta())
				continue;

			auto posInLightSpace = worldToLightVec[i](hitPos);
			Normalf dirInLight = worldToLightVec[i](matRayDirInWorld);
			sumPD += lights[i]->PDF(posInLightSpace, dirInLight) * scaleFactor;
		}
	}

	// material ray
	ERay matRay(hitPos, matRayDirInWorld);

	// Russian Roulette
	const RGBf matWeight = abs(mat_w_in.z) / sumPD * matF;
	pathThroughput *= matWeight;
	float continueP = bsdf->IsDelta() ? 1.f : min(1.f, pathThroughput.Illumination());
	if (Math::Rand_F() > continueP)
		return RGBf(0.f);

	const RGBf matRayColor = Trace(matRay, depth + 1, pathThroughput / continueP);

	return matWeight / continueP * matRayColor;
}
