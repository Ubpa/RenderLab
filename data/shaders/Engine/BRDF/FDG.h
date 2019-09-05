#ifndef BRDF_FDG_H
#define BRDF_FDG_H

#include "../../Math/quat.h"

#define DEFAULT_REFLECTANCE 0.04

// ------------------------------ 接口 ------------------------------

float SchlickGGX_D(vec3 norm, vec3 h, float roughness);
float SchlickGGX_G1(vec3 norm, vec3 w, float roughness);
float SchlickGGX_SmithG(vec3 norm, vec3 wo, vec3 wi, float roughness);
// Disney : reduce "hotness" for analytic light sources
float SchlickGGX_SmithG_Analytic(vec3 norm, vec3 wo, vec3 wi, float roughness);
vec3 SchlickGGX_Sample(vec2 Xi, vec3 norm, float roughness);

vec3 MetalWorkflowF0(vec3 albedo, float metallic);
vec3 MetalWorkflowF0(vec3 reflectance, vec3 albedo, float metallic);
vec3 SchlickFr(vec3 w, vec3 h, vec3 albedo, float metallic);
vec3 SchlickFrR(vec3 wo, vec3 norm, vec3 F0, float roughness);
float DisneyDiffuseFr(vec3 norm, vec3 wo, vec3 wi, float linearRoughness);

// ------------------------------ 实现 ------------------------------

float SchlickGGX_D(vec3 norm, vec3 h, float roughness){
	float NoH = saturate(dot(norm, h));
	
	float alpha = roughness * roughness;
	
	float alpha2 = alpha * alpha;
	float cos2Theta = NoH * NoH;
	
	float t = (alpha2 - 1) * cos2Theta + 1;
	
	return alpha2 / (PI * t * t);
}

float SchlickGGX_G1(vec3 norm, vec3 w, float roughness) {
	float alpha = roughness * roughness;
	float k = alpha / 2; // fix smith model ggx
	
	float NoW = max(0, dot(norm, w));
	return NoW / (NoW * (1 - k) + k);
}

float SchlickGGX_SmithG(vec3 norm, vec3 wo, vec3 wi, float roughness){
	return SchlickGGX_G1(norm, wo, roughness) * SchlickGGX_G1(norm, wi, roughness);
}

float SchlickGGX_SmithG_Analytic(vec3 norm, vec3 wo, vec3 wi, float roughness) {
	float remappedRoughness = (roughness + 1) * 0.5;
	return SchlickGGX_SmithG(norm, wo, wi, remappedRoughness);
}

vec3 SchlickGGX_Sample(vec2 Xi, vec3 norm, float roughness) {
	float a = roughness * roughness;

	float phi = TWO_PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space H vector to world-space sample vector
	vec4 rot = Quat_ZTo(norm);
	return Quat_Rotate(rot, H);
}

vec3 MetalWorkflowF0(vec3 reflectance, vec3 albedo, float metallic) {
	return mix(reflectance, albedo, metallic);
}

vec3 MetalWorkflowF0(vec3 albedo, float metallic){
	return MetalWorkflowF0(vec3(DEFAULT_REFLECTANCE), albedo, metallic);
}

vec3 SchlickFr(vec3 w, vec3 h, vec3 albedo, float metallic) {
	vec3 F0 = MetalWorkflowF0(albedo, metallic);
	float HoW = dot(h, w);
	return F0 + exp2((-5.55473f * HoW - 6.98316f) * HoW) * (vec3(1.0f) - F0);
}

vec3 SchlickFrR(vec3 wo, vec3 norm, vec3 F0, float roughness) {
	float cosTheta = max(dot(wo, norm), 0);
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DisneyDiffuseFr(vec3 norm, vec3 wo, vec3 wi, float linearRoughness) {
	vec3 h = normalize(wo + wi);
	float HoWi = dot(h, wi);
	float HoWi2 = HoWi * HoWi;
	
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);
	
	float energyBias = mix(0.f, 0.5f, linearRoughness);
	float energyFactor = mix(1.f, 1.f / 1.51f, linearRoughness);
	float fd90 = energyBias + 2.f * HoWi2 * linearRoughness;
	float lightScatter = 1.f + fd90 * pow(1.f - NoWi * NoWi, 5);
	float viewScatter = 1.f + fd90 * pow(1.f - NoWo * NoWo, 5);
	
	return lightScatter * viewScatter * energyFactor;
}

#endif // !BRDF_DIFFUSE_H
