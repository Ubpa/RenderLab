#include <CppUtil/Engine/PathTracer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayIntersector.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Material.h>
#include <CppUtil/Engine/Light.h>
#include <CppUtil/Engine/LightBase.h>
#include <CppUtil/Engine/BSDF_Diffuse.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

PathTracer::PathTracer(Scene::Ptr scene)
	: RayTracer(scene), sampleNumForAreaLight(5), maxDepth(10) { }

vec3 PathTracer::Trace(Ray::Ptr ray, int depth) {
	rayIntersector->SetRay(ray);
	Rst closestRst;
	FindClosetSObj(scene->GetRootSObj(), ray, closestRst);
	if (!closestRst.closestSObj) {
		float t = 0.5f * (normalize(ray->GetDir()).y + 1.0f);
		vec3 white(1.0f, 1.0f, 1.0f);
		vec3 blue(0.5f, 0.7f, 1.0f);
		return t * white + (1 - t)*blue;
		// return vec3(0);
	}

	// 错误情况
	auto material = closestRst.closestSObj->GetComponent<Material>();
	if (material == nullptr)
		return vec3(1, 0, 1);

	// 现只支持 BSDF
	BSDF_Diffuse::Ptr bsdf = material->GetMat();
	if (bsdf == nullptr)
		return vec3(1, 0, 1);

	vec3 emitL = bsdf->GetEmission();

	const vec3 hitPos = ray->At(ray->GetTMax());

	auto const surfaceToObject = Math::GenCoordSpace(closestRst.n);
	auto const objectToSurface = transpose(surfaceToObject);

	// w_out 处于表面坐标系
	auto w_out = normalize(objectToSurface * (-ray->GetDir()));

	vec3 sumLightL(0);
	
	// 计算与灯光相关的数据
	vector<LightBase::Ptr> lights;
	vector<mat3> objectToLightVec;// 只需要旋转方向，所以使用 mat3
	vector<mat3> lightToObjectVec;// 只需要旋转方向，所以使用 mat3
	vector<mat3> lightToWorldVec;// 只需要旋转方向，所以使用 mat3
	vector<vec3> posInLightSpaceVec;

	auto objectToWorld = closestRst.closestSObj->GetLocalToWorldMatrix();
	auto worldToObject = inverse(objectToWorld);
	vec4 hitPos4 = vec4(hitPos, 1);

	for (auto lightComponent : scene->GetLights()) {
		mat4 lightToWorld = lightComponent->GetLightToWorldMatrixWithoutScale();
		mat4 worldToLight = inverse(lightToWorld);
		mat4 objectToLight = worldToLight * objectToWorld;

		lightToWorldVec.push_back(lightToWorld);
		lights.push_back(lightComponent->GetLight());
		objectToLightVec.push_back(objectToLight);
		lightToObjectVec.push_back(worldToObject * lightToWorld);
		posInLightSpaceVec.push_back(objectToLight * hitPos4);
	}
	int lightNum = lights.size();
	
	if (!bsdf->IsDelta()) {
		const vec3 hitPosInWorld = objectToWorld * vec4(hitPos, 1);

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

				const vec3 dirInObject = lightToObjectVec[i] * dir_ToLight;
				// w_in 处于表面坐标系，应该是单位向量
				const vec3 w_in = objectToSurface * dirInObject;

				// 多重重要性采样 Multiple Importance Sampling (MIS)
				float sumPD = bsdf->PDF(w_out, w_in) + sampleNum * PD;
				for (int k = 0; k < lightNum;k++) {
					if (k != i) {
						int sampleNum = lights[k]->IsDelta() ? 1 : sampleNumForAreaLight;
						vec3 dir = objectToLightVec[k] * dirInObject;
						sumPD += sampleNum * lights[k]->PDF(posInLightSpaceVec[i], dir_ToLight);
					}
				}

				// 在碰撞表面的坐标系计算 dot(n, w_in) 很简单，因为 n = (0, 0, 1)
				const float cos_theta = w_in.z;

				// evaluate surface bsdf
				const vec3 f = bsdf->F(w_out, w_in);

				// dirInWorld 应该是单位向量
				const vec3 dirInWorld = lightToWorldVec[i] * dir_ToLight;
				vec3 originInWorld = hitPosInWorld + Math::EPSILON * dirInWorld;
				// shadowRay 处于世界坐标
				Ray::Ptr shadowRay = ToPtr(new Ray(originInWorld, dirInWorld));
				shadowRay->SetTMax(dist_ToLight - Math::EPSILON);
				Rst shadowRst;
				// 应该使用一个优化过的函数
				// 设置 ray 的 tMax，然后只要找到一个碰撞后即可返回，无需找到最近的
				FindClosetSObj(scene->GetRootSObj(), shadowRay, shadowRst);
				if (!shadowRst.closestSObj)
					sumLightL += (cos_theta / sumPD) * f * lightL;
			}
		}
	}

	// 深度，一定概率丢弃
	float depthP = depth > maxDepth ? 0.5f : 1.0f;
	if (Math::Rand_F() <= depthP)
		return emitL + sumLightL;

	// 积分方程，一定概率丢弃
	vec3 mat_w_in;
	float matPD;
	// matPD 一定大于 0
	const vec3 matF = bsdf->Sample_f(w_out, mat_w_in, matPD);

	float terminateProbability = 0.f;
	// Pareto principle : 2-8 principle
	// 0.2 * cos(PI / 2 * 0.8) == 0.0618
	if (!bsdf->IsDelta() && Math::Illum(matF) * abs(mat_w_in.z) < 0.0618f)
		terminateProbability = 0.8f;

	if (Math::Rand_F() < terminateProbability)
		return emitL + sumLightL;

	float sumPD = matPD;
	vec3 matRayDir = surfaceToObject * mat_w_in;
	if (bsdf->IsDelta()) {
		for (int i = 0; i < lightNum; i++) {
			int sampleNum = lights[i]->IsDelta() ? 1 : sampleNumForAreaLight;
			vec3 dir = objectToLightVec[i] * matRayDir;
			sumPD += sampleNum * lights[i]->PDF(posInLightSpaceVec[i], dir);
		}
	}

	float cosTheta = mat_w_in.z;

	Ray::Ptr matRay = ToPtr(new Ray(hitPos + Math::EPSILON * matRayDir, matRayDir));
	const vec3 matRayColor = Trace(matRay, depth + 1);

	vec3 matL = abs(cosTheta) / (sumPD*(1.f - terminateProbability)*depthP) * matF * matRayColor;

	return emitL + sumLightL + matL;
}
