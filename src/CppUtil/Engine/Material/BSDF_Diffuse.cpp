#include <CppUtil/Engine/BSDF_Diffuse.h>
#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;

vec3 BSDF_Diffuse::F(const vec3 & wo, const vec3 & wi) {
	return albedo * (1.0f / PI);
}

vec3 BSDF_Diffuse::Sample_f(const vec3 & wo, vec3 & wi, float & PD) {
	wi = sampler.GetSample(PD);
	if (wo.z < 0)
		wi.z *= -1;

	return albedo * (1.0f / PI);
}

float BSDF_Diffuse::PDF(const vec3 & wo, const vec3 & wi) {
	return wi.z / PI;
}
