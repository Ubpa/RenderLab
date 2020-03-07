#include <Engine/PathTracer.h>

#include <Engine/BVHAccel.h>

#include <Engine/Intersector/ClosestIntersector.h>
#include <Engine/Intersector/VisibilityChecker.h>

#include <Engine/Scene.h>
#include <Engine/SObj.h>

#include <Engine/CmptMaterial.h>
#include <Engine/BSDF.h>

#include <Engine/CmptLight.h>
#include <Engine/Light.h>

#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

PathTracer::PathTracer()
	:
	maxDepth(20),
	closestIntersector(ClosestIntersector::New()),
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
		const auto worldToLight = lightToWorld.inverse();

		worldToLightVec.push_back(worldToLight);
		lightToWorldVec.push_back(lightToWorld);
	}
}

const rgbf PathTracer::Trace(Ray & ray, int depth, rgbf pathThroughput) {
	closestIntersector->Init(&ray);
	closestIntersector->Visit(bvhAccel);
	auto closestRst = closestIntersector->GetRst();
	if (!closestRst.closestSObj) {
		rgbf Le(0.f);
		for (auto light : lights)
			Le += light->Le(ray);

		return Le;
	}

	const pointf3 hitPos = ray.EndPos();

	auto cmptMaterial = closestRst.closestSObj->GetComponent<CmptMaterial>();
	if (!cmptMaterial || !cmptMaterial->material)
		return rgbf(0.f);

	auto bsdf = dynamic_pointer_cast<BSDF>(cmptMaterial->material);
	if (bsdf == nullptr)
		return rgbf(0.f);

	bsdf->ChangeNormal(closestRst.texcoord, closestRst.tangent, closestRst.n);

	const auto surfaceToWorld = closestRst.n.coord_space();
	const auto worldToSurface = surfaceToWorld.transpose();

	// w_out 处于表面坐标系，向外
	const normalf w_out = (worldToSurface * (-ray.d)).normalize().cast_to<normalf>();

	rgbf emitL = depth == 0 ? bsdf->Emission(w_out) : rgbf(0.f);

	// SampleLightMode mode = depth > 0 ? SampleLightMode::RandomOne : SampleLightMode::ALL;
	SampleLightMode mode = SampleLightMode::RandomOne;
	const rgbf lightL = SampleLight(hitPos, worldToSurface, bsdf, w_out, closestRst.texcoord, SampleLightMode::RandomOne);

	const rgbf matL = SampleBSDF(bsdf, mode, w_out, surfaceToWorld, closestRst.texcoord, hitPos, depth, pathThroughput);

	return emitL + lightL + matL;
}

const rgbf PathTracer::SampleLightImpl(
	const int lightID,
	const pointf3 & posInWorldSpace,
	const pointf3 & posInLightSpace,
	const matf3 & worldToSurface,
	const Ptr<BSDF> bsdf,
	const normalf & w_out,
	const pointf2 & texcoord,
	float factorPD
) const
{
	auto const light = lights[lightID];
	auto const & lightToWorld = lightToWorldVec[lightID];
	const int lightNum = static_cast<int>(lights.size());

	float dist_ToLight;
	float PD;// 概率密度
	normalf dir_ToLight;
	// dir_ToLight 是单位向量
	const rgbf lightL = light->Sample_L(posInLightSpace, dir_ToLight, dist_ToLight, PD);
	PD *= factorPD;
	if (PD == 0)
		return rgbf(0.f);

	const normalf dirInWorld = (lightToWorld * dir_ToLight).normalize();

	// w_in 处于表面坐标系，应该是单位向量
	// const normalf w_in = (worldToSurface * dirInWorld).normalize();
	const normalf w_in = (transformf(worldToSurface) * dirInWorld).normalize();

	// evaluate surface bsdf
	const rgbf f = bsdf->F(w_out, w_in, texcoord);
	if (f.rmv_epsilon().is_all_zero())
		return rgbf(0.f);

	// shadow ray 处于世界坐标
	Ray shadowRay(posInWorldSpace, dirInWorld.cast_to<vecf3>());
	visibilityChecker->Init(shadowRay, dist_ToLight - 0.001f);
	visibilityChecker->Visit(bvhAccel);
	auto shadowRst = visibilityChecker->GetRst();
	if (shadowRst.IsIntersect())
		return rgbf(0.f);

	auto weight = (abs(w_in[2]) / PD) * f;
	return weight * lightL;
}

const rgbf PathTracer::SampleLight(
	const pointf3 & posInWorldSpace,
	const matf3 & worldToSurface,
	const Ptr<BSDF> bsdf,
	const normalf & w_out,
	const pointf2 & texcoord,
	const SampleLightMode mode
) const
{
	if (bsdf->IsDelta())
		return rgbf(0.f);

	int lightNum = static_cast<int>(lights.size());
	rgbf rst(0.f);

	switch (mode)
	{
	case SampleLightMode::ALL: {
		for (int i = 0; i < lightNum; i++) {
			auto posInLightSpace = worldToLightVec[i] * posInWorldSpace;
			rst += SampleLightImpl(i, posInWorldSpace, posInLightSpace, worldToSurface, bsdf, w_out, texcoord, 1.f);
		}

		break;
	}
	case SampleLightMode::RandomOne: {
		int lightID = Math::Rand_I() % lightNum;
		auto posInLightSpace = worldToLightVec[lightID] * posInWorldSpace;
		rst = SampleLightImpl(lightID, posInWorldSpace, posInLightSpace, worldToSurface, bsdf, w_out, texcoord, 1.f / lightNum);
		break;
	}
	}

	return rst;
}

const rgbf PathTracer::SampleBSDF(
	const Ptr<BSDF> bsdf,
	const SampleLightMode mode,
	const normalf & w_out,
	const matf3 & surfaceToWorld,
	const pointf2 & texcoord,
	const pointf3 & hitPos,
	const int depth,
	rgbf pathThroughput
)
{
	if (depth + 1 >= maxDepth)
		return rgbf(0.f);

	normalf mat_w_in;
	float matPD;
	const rgbf matF = bsdf->Sample_f(w_out, texcoord, mat_w_in, matPD);
	if (matPD <= 0)
		return rgbf(0.f);

	const normalf matRayDirInWorld = (transformf(surfaceToWorld) * mat_w_in).normalize();
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

			auto posInLightSpace = worldToLightVec[i] * hitPos;
			normalf dirInLight = worldToLightVec[i] * matRayDirInWorld;
			sumPD += lights[i]->PDF(posInLightSpace, dirInLight) * scaleFactor;
		}
	}

	// material ray
	Ray matRay(hitPos, matRayDirInWorld.cast_to<vecf3>());

	// Russian Roulette
	const rgbf matWeight = abs(mat_w_in[2]) / sumPD * matF;
	pathThroughput *= matWeight;
	//float continueP = bsdf->IsDelta() ? 1.f : std::min(1.f, pathThroughput.illumination());
	float continueP = bsdf->IsDelta() ? 1.f : std::min(1.f, (pathThroughput[0]+ pathThroughput[1]+ pathThroughput[2])/3.f);
	if (Math::Rand_F() > continueP)
		return rgbf(0.f);

	const rgbf matRayColor = Trace(matRay, depth + 1, pathThroughput / continueP);

	return matWeight / continueP * matRayColor;
}
