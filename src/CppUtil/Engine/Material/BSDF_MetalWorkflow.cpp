#include <CppUtil/Engine/BSDF_MetalWorkflow.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

vec3 BSDF_MetalWorkflow::F(const vec3 & wo, const vec3 & wi) {
	auto diffuse = albedo / Math::PI;
	return (1 - metallic)*diffuse + MS_BRDF(wo, wi);
}

float BSDF_MetalWorkflow::PDF(const vec3 & wo, const vec3 & wi) {
	vec3 h = normalize(wo + wi);
	return NDF(h) / 4.0f;
	//return 1.0f / (2.0f * Math::PI);
}

vec3 BSDF_MetalWorkflow::Sample_f(const vec3 & wo, vec3 & wi, float & pd) {
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	// 根据 NDF 采样
	float alpha = roughness * roughness;
	float cosTheta2 = (1 - Xi1) / (Xi1*(alpha*alpha - 1) + 1);
	float cosTheta = sqrt(cosTheta2);
	float sinTheta = sqrt(1 - cosTheta2);
	float phi = 2 * Math::PI*Xi2;
	vec3 h(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	wi = reflect(-wo, h);
	if (wi.z <= 0) {
		pd = 0;
		return vec3(0);
	}
	pd = NDF(h) / 4.0f;

	/*
	// 均匀采样
	float cosTheta = Xi1;
	float sinTheta = sqrt(1 - cosTheta * cosTheta);
	float phi = 2 * Math::PI * Xi2;
	wi = vec3(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	vec3 h = normalize(wo + wi);
	pd = 1.0f / (2.0f * Math::PI);
	*/

	auto diffuse = albedo / Math::PI;
	return (1 - metallic)*diffuse + MS_BRDF(wo, wi);
}

vec3 BSDF_MetalWorkflow::MS_BRDF(const vec3 & wo, const vec3 & wi) {
	vec3 h = normalize(wo + wi);
	return NDF(h)*Fr(wi, h)*G(wo, wi) / (4 * wo.z*wi.z);
}

float BSDF_MetalWorkflow::NDF(const glm::vec3 & h) {
	//  GGX/Trowbridge-Reitz

	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NoH = h.z;
	return alpha2 / (Math::PI*pow(NoH*NoH*(alpha2 - 1) + 1, 2));
}

vec3 BSDF_MetalWorkflow::Fr(const glm::vec3 & wi, glm::vec3 & h) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	vec3 F0 = mix(vec3(0.04f), albedo, metallic);
	float HoWi = dot(h, wi);
	return F0 + (vec3(1.0f) - F0) * pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi);
}

float BSDF_MetalWorkflow::G(const glm::vec3 & wo, const glm::vec3 & wi) {
	// Schlick, remap roughness and k

	// k = alpha / 2
	// direct light: alpha = pow( (roughness + 1) / 2, 2)
	// IBL(image base lighting) : alpha = pow( roughness, 2)

	if (wo.z <= 0 || wi.z <= 0)
		return 0;

	float k = pow(roughness + 1, 2) / 8.f;
	float G1_wo = wo.z / (wo.z*(1 - k) + k);
	float G1_wi = wi.z / (wi.z*(1 - k) + k);
	return G1_wo * G1_wi;
}
