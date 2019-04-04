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
	bvhAccel(ToPtr(new BVHAccel)),
	rayIntersector(ToPtr(new RayIntersector)),
	visibilityChecker(ToPtr(new VisibilityChecker))
{ }

void PathTracer::Init(Scene::Ptr scene) {
	lights.clear();
	worldToLightVec.clear();
	lightToWorldVec.clear();
	lightToIdx.clear();

	auto cmptLights = scene->GetCmptLights();
	for (int i = 0; i < cmptLights.size(); i++) {
		auto cmptLight = cmptLights[i];
		auto light = cmptLight->light;

		lightToIdx[light] = i;

		lights.push_back(light);

		const auto lightToWorld = cmptLight->GetLightToWorldMatrixWithoutScale();
		const auto worldToLight = lightToWorld.Inverse();

		worldToLightVec.push_back(worldToLight);
		lightToWorldVec.push_back(lightToWorld);
	}

	bvhAccel->Init(scene->GetRoot());
}

const RGBf PathTracer::Trace(ERay & ray, int depth, RGBf pathThroughput) {
	rayIntersector->Init(&ray);
	bvhAccel->Accept(rayIntersector);
	auto & closestRst = rayIntersector->GetRst();
	if (!closestRst.closestSObj) {
		return RGBf(0);
		/*
		float t = 0.5f * (normalize(ray->GetDir()).y + 1.0f);
		vec3 white(1.0f, 1.0f, 1.0f);
		vec3 blue(0.5f, 0.7f, 1.0f);
		return t * white + (1 - t)*blue;
		*/
	}

	const Point3 hitPos = ray.EndPos();

	const size_t lightNum = lights.size();
	vector<Point3> posInLightSpaceVec(lightNum);

	for (size_t i = 0; i < lightNum; i++)
		posInLightSpaceVec[i] = worldToLightVec[i](hitPos);

	RGBf emitL(0.f);
	auto cmptLight = closestRst.closestSObj->GetComponent<CmptLight>();
	if (cmptLight) {
		int idx = lightToIdx[cmptLight->light];
		emitL = cmptLight->light->GetMaxL(worldToLightVec[idx](ray.o));
	}

	auto material = closestRst.closestSObj->GetComponent<CmptMaterial>();
	if (material == nullptr)
		return emitL;

	auto bsdf = BSDF::Ptr::Cast(material->material);
	if (bsdf == nullptr)
		return emitL;

	emitL += bsdf->GetEmission();

	bsdf->ChangeNormal(closestRst.texcoord, closestRst.tangent, closestRst.n);

	const auto surfaceToWorld = closestRst.n.GenCoordSpace();
	const auto worldToSurface = surfaceToWorld.Transpose();

	// w_out 处于表面坐标系，向外
	const Normalf w_out = (worldToSurface * (-ray.d)).Norm();

	// SampleLightMode mode = depth > 0 ? SampleLightMode::RandomOne : SampleLightMode::ALL;
	SampleLightMode mode = SampleLightMode::RandomOne;
	const RGBf lightL = SampleLight(ray, hitPos, posInLightSpaceVec, worldToSurface, bsdf, w_out, closestRst.texcoord, SampleLightMode::RandomOne);

	const RGBf matL = SampleBSDF(bsdf, mode, w_out, surfaceToWorld, closestRst.texcoord, posInLightSpaceVec, ray, hitPos, depth, pathThroughput);

	return emitL + lightL + matL;
}

const RGBf PathTracer::SampleLightImpl(
	ERay & ray,
	const int lightID,
	const Point3 & posInWorldSpace,
	const Point3 & posInLightSpace,
	const Mat3f & worldToSurface,
	const Basic::Ptr<BSDF> bsdf,
	const Normalf & w_out,
	const Point2 & texcoord,
	const float factorPD
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
	if (PD == 0)
		return RGBf(0.f);

	PD *= factorPD;

	const Normalf dirInWorld = lightToWorld(dir_ToLight).Norm();

	// shadow ray 处于世界坐标
	ray.Init(posInWorldSpace, dirInWorld);
	visibilityChecker->Init(ray, dist_ToLight - 0.001f);
	bvhAccel->Accept(visibilityChecker);
	auto shadowRst = visibilityChecker->GetRst();
	if (shadowRst.IsIntersect())
		return RGBf(0);

	// w_in 处于表面坐标系，应该是单位向量
	const Normalf w_in = (worldToSurface * dirInWorld).Norm();

	// 多重重要性采样 Multiple Importance Sampling (MIS)
	if (!light->IsDelta()) {
		for (int k = 0; k < lightNum; k++) {
			if (k != lightID && !lights[k]->IsDelta()) {
				const auto dirInLight = worldToLightVec[k](dirInWorld).Norm();
				PD += lights[k]->PDF(posInLightSpace, dirInLight) * factorPD;
			}
		}
		PD += bsdf->PDF(w_out, w_in, texcoord);
	}

	// 在碰撞表面的坐标系计算 dot(n, w_in) 很简单，因为 n = (0, 0, 1)
	const float cos_theta = max(w_in.z, 0.f);

	// evaluate surface bsdf
	const RGBf f = bsdf->F(w_out, w_in, texcoord);

	auto weight = (cos_theta / PD) * f;
	return weight * lightL;
}

const RGBf PathTracer::SampleLight(
	ERay & ray,
	const Point3 & posInWorldSpace,
	const std::vector<Point3> & posInLightSpaceVec,
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
		for (int i = 0; i < lightNum; i++)
			rst += SampleLightImpl(ray, i, posInWorldSpace, posInLightSpaceVec[i], worldToSurface, bsdf, w_out, texcoord, 1.f);

		break;
	}
	case SampleLightMode::RandomOne: {
		int lightID = Math::Rand_I() % lightNum;
		auto const & posInLightSpace = posInLightSpaceVec[lightID];

		rst = SampleLightImpl(ray, lightID, posInWorldSpace, posInLightSpace, worldToSurface, bsdf, w_out, texcoord, 1.f / lightNum);
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
	const std::vector<Point3> & posInLightSpaceVec,
	ERay & ray,
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

	const Normalf matRayDirInWorld = (surfaceToWorld * mat_w_in).Norm();
	const float abs_cosTheta = abs(mat_w_in.z);
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

			Normalf dirInLight = worldToLightVec[i](matRayDirInWorld);
			sumPD += lights[i]->PDF(posInLightSpaceVec[i], dirInLight) * scaleFactor;
		}
	}

	// material ray
	ray.Init(hitPos, matRayDirInWorld);

	// Russian Roulette
	const RGBf matWeight = abs_cosTheta / sumPD * matF;
	pathThroughput *= matWeight;
	float continueP = bsdf->IsDelta() ? 1.f : min(1.f, pathThroughput.Illumination());
	if (Math::Rand_F() > continueP)
		return RGBf(0.f);

	const RGBf matRayColor = Trace(ray, depth + 1, pathThroughput / continueP);

	return matWeight / continueP * matRayColor;
}
