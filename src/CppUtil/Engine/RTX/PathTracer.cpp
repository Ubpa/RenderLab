#include <CppUtil/Engine/PathTracer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Material.h>
#include <CppUtil/Engine/Light.h>
#include <CppUtil/Engine/LightBase.h>
#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/Math.h>
#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

PathTracer::PathTracer(Scene::Ptr scene)
	: RayTracer(scene), sampleNumForAreaLight(5), maxDepth(10) { }

vec3 PathTracer::Trace(Ray::Ptr ray, int depth) {
	Rst closestRst = FindClosetSObj(scene->GetRootSObj(), ray);
	if (!closestRst.closestSObj) {
		return vec3(0, 0, 0);

		float t = 0.5f * (normalize(ray->GetDir()).y + 1.0f);
		vec3 white(1.0f, 1.0f, 1.0f);
		vec3 blue(0.5f, 0.7f, 1.0f);
		return t * white + (1 - t)*blue;
	}

	// 错误情况
	auto material = closestRst.closestSObj->GetComponent<Material>();
	if (material == nullptr)
		return vec3(1, 0, 1);

	// 现只支持 BSDF
	BSDF::Ptr bsdf = material->GetMat();
	if (bsdf == nullptr)
		return vec3(1, 0, 1);

	vec3 emitL = depth == 0 ? bsdf->GetEmission() : vec3(0);

	const vec3 hitPos = ray->At(ray->GetTMax());

	auto const surfaceToWorld = Math::GenCoordSpace(closestRst.n);
	auto const worldToSurface = transpose(surfaceToWorld);

	// w_out 处于表面坐标系
	auto w_out = normalize(worldToSurface * (-ray->GetDir()));

	vec3 sumLightL(0);
	
	// 计算与灯光相关的数据
	vector<LightBase::Ptr> lights;
	vector<mat3> worldToLightVec;// 只需要旋转方向，所以使用 mat3
	vector<mat3> lightToWorldVec;// 只需要旋转方向，所以使用 mat3
	vector<vec3> posInLightSpaceVec;

	vec4 hitPos4 = vec4(hitPos, 1);

	for (auto lightComponent : scene->GetLights()) {
		lights.push_back(lightComponent->GetLight());

		mat4 lightToWorld = lightComponent->GetLightToWorldMatrixWithoutScale();
		mat4 worldToLight = inverse(lightToWorld);

		lightToWorldVec.push_back(lightToWorld);
		worldToLightVec.push_back(worldToLight);
		posInLightSpaceVec.push_back(worldToLight * hitPos4);
	}
	int lightNum = lights.size();
	
	if (!bsdf->IsDelta()) {
		vec3 dir_ToLight; // 把变量放在这里减少初始化次数

		for (int i = 0; i < lightNum; i++) {
			auto const light = lights[i];

			const int sampleNum = light->IsDelta() ? 1 : sampleNumForAreaLight;

			for (int j = 0; j < sampleNum; j++) {
				float dist_ToLight;
				float PD;// 概率密度
				// dir_ToLight 是单位向量
				const vec3 lightL = light->Sample_L(posInLightSpaceVec[i], dir_ToLight, dist_ToLight, PD);

				// 在 MIT 15-462 中为 if (w_in.z < 0) continue;
				// 理由就是在 BSDF 不为 Delta 的情况下，不应该出现 w_in 到表面下边的情况
				// 觉得可能有些材质还是允许这种情况发生的
				// 所以更合理的是 PD <= 0
				if (PD <= 0)
					continue;

				// dirInWorld 应该是单位向量
				const vec3 dirInWorld = lightToWorldVec[i] * dir_ToLight;
				// w_in 处于表面坐标系，应该是单位向量
				const vec3 w_in = worldToSurface * dirInWorld;

				// 多重重要性采样 Multiple Importance Sampling (MIS)
				float sumPD = bsdf->PDF(w_out, w_in) + sampleNum * PD;
				for (int k = 0; k < lightNum; k++) {
					if (k != i) {
						int sampleNum = lights[k]->IsDelta() ? 1 : sampleNumForAreaLight;
						vec3 dir = worldToLightVec[k] * dirInWorld;
						sumPD += sampleNum * lights[k]->PDF(posInLightSpaceVec[i], dir);
					}
				}

				// 在碰撞表面的坐标系计算 dot(n, w_in) 很简单，因为 n = (0, 0, 1)
				const float cos_theta = w_in.z;

				// evaluate surface bsdf
				const vec3 f = bsdf->F(w_out, w_in);

				vec3 originInWorld = hitPos + Math::EPSILON * closestRst.n;
				// shadowRay 处于世界坐标
				Ray::Ptr shadowRay = ToPtr(new Ray(originInWorld, dirInWorld));
				shadowRay->SetTMax(dist_ToLight/length(dirInWorld) - 0.001f);
				// 应该使用一个优化过的函数
				// 设置 ray 的 tMax，然后只要找到一个碰撞后即可返回，无需找到最近的
				Rst shadowRst = FindClosetSObj(scene->GetRootSObj(), shadowRay);
				if (!shadowRst.closestSObj)
					sumLightL += (cos_theta / sumPD) * f * lightL;
			}
		}
	}

	// 采样 BSDF
	vec3 mat_w_in;
	float matPD;
	const vec3 matF = bsdf->Sample_f(w_out, mat_w_in, matPD);
	const vec3 matRayDirInWorld = surfaceToWorld * mat_w_in;
	const float abs_cosTheta = abs(mat_w_in.z);

	if (bsdf->IsDelta()) {
		for (int i = 0; i < lightNum; i++) {
			vec3 dir = worldToLightVec[i] * matRayDirInWorld;
			sumLightL += lights[i]->GetL(posInLightSpaceVec[i], dir);
		}
		sumLightL *= abs_cosTheta;
	}

	// 深度，丢弃概率
	float depthP = depth > maxDepth ? 0.5f : 0.f;
	if (Math::Rand_F() < depthP)
		return emitL + sumLightL;


	if(matPD <= 0)
		return emitL + sumLightL;

	// 一定概率丢弃
	float terminateProbability = 0.f;
	if (!bsdf->IsDelta() && Math::Illum(matF) < 0.2f)
		terminateProbability = 0.8f;

	if (Math::Rand_F() < terminateProbability)
		return emitL + sumLightL;

	// 重要性采样
	float sumPD = matPD;
	if (!bsdf->IsDelta()) {
		for (int i = 0; i < lightNum; i++) {
			int sampleNum = lights[i]->IsDelta() ? 1 : sampleNumForAreaLight;
			vec3 dir = worldToLightVec[i] * matRayDirInWorld;
			sumPD += sampleNum * lights[i]->PDF(posInLightSpaceVec[i], dir);
		}
	}

	Ray::Ptr matRay = ToPtr(new Ray(hitPos + Math::EPSILON * matRayDirInWorld, matRayDirInWorld));
	const vec3 matRayColor = Trace(matRay, depth + 1);

	vec3 matL = abs_cosTheta / (sumPD * (1.f - terminateProbability) * (1.f - depthP)) * matF * matRayColor;

	return emitL + sumLightL + matL;
}
