#include <CppUtil/Engine/BSDF_Diffuse.h>
#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;

vec3 BSDF_Diffuse::F(const vec3 & wo, const vec3 & wi) {
	if (wi.z < 0)
		return vec3(0);
	else
		return albedo * (1.0f / PI);
}

vec3 BSDF_Diffuse::Sample_f(const vec3 & wo, vec3 & wi, float & PD) {
	float Xi1 = Rand_F();
	float Xi2 = Rand_F();

	float a = sqrt(1 - Xi1 * Xi1);
	wi.x = a * cos(2 * PI * Xi2);
	wi.y = a * sin(2 * PI * Xi2);
	wi.z = Xi1;
	PD = this->PDF(wo, wi);

	return albedo * (1.0f / PI);
}

float BSDF_Diffuse::PDF(const vec3 & wo, const vec3 & wi) {
	return 1.0f / (2.0f * PI);
}
