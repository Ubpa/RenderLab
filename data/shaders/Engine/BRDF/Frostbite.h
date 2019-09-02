#ifndef BRDF_FROSTBITE_H
#define BRDF_FROSTBITE_H

#include "FDG.h"

// ------------------------------ 接口 ------------------------------

void BRDF_Frostbite(out vec3 fd, out vec3 fs, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
vec3 BRDF_Frostbite(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

// We have a better approximation of the off specular peak
// but due to the other approximations we found this one performs better.
// N is the normal direction
// R is the mirror vector
// This approximation works fine for G smith correlated and uncorrelated
vec3 Frostbite_SpecularDominantDir(vec3 norm, vec3 R, float roughness);

// N is the normal direction
// V is the view vector
// NdotV is the cosine angle between the view vector and the normal
vec3 Frostbite_DiffuseDominantDir(vec3 N, vec3 V, float roughness);

// ------------------------------ 实现 ------------------------------

void BRDF_Frostbite(out vec3 fd, out vec3 fs, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness){
	vec3 wh = normalize(wo + wi);
	
	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_SmithG(norm, wo, wi, roughness);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);
	
	fd = (1 - metallic) * albedo * INV_PI * DisneyDiffuseFr(norm, wo, wi, roughness);
	fs = D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
}

vec3 BRDF_Frostbite(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	vec3 fd, fs;
	BRDF_Frostbite(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	return fd + fs;
}

vec3 Frostbite_SpecularDominantDir(vec3 norm, vec3 R, float roughness) {
    float smoothness = clamp(1 - roughness, 0, 1);
    float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
    // The result is not normalized as we fetch in a cubemap
    return mix(norm, R, lerpFactor);
}

vec3 Frostbite_DiffuseDominantDir(vec3 N, vec3 V, float roughness) {
	float NoV = dot(N, V);
	float a = 1.02341f * roughness - 1.51174f;
	float b = -0.511705f * roughness + 0.755868f;
	float lerpFactor = clamp((NoV * a + b) * roughness, 0, 1);
	// The result is not normalized as we fetch in a cubemap
	return mix(N, V, lerpFactor);
}

#endif // !BRDF_FROSTBITE_H
