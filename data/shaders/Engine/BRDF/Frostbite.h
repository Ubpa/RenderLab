#ifndef BRDF_FROSTBITE_H
#define BRDF_FROSTBITE_H

#include "FDG.h"

// ------------------------------ 接口 ------------------------------

void BRDF_Frostbite(out vec3 fd, out vec3 fs, vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness);
// roughness is linear
vec3 BRDF_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness);

// diffuse
vec3 BRDFd_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness);
// specular
vec3 BRDFs_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness);

// We have a better approximation of the off specular peak
// but due to the other approximations we found this one performs better.
// N is the normal direction
// R is the mirror vector
// This approximation works fine for G smith correlated and uncorrelated
vec3 Frostbite_SpecularDominantDir(vec3 N, vec3 R, float roughness);

// N is the normal direction
// V is the view vector
// NdotV is the cosine angle between the view vector and the normal
vec3 Frostbite_DiffuseDominantDir(vec3 N, vec3 V, float roughness);

// ------------------------------ 实现 ------------------------------

void BRDF_Frostbite(out vec3 fd, out vec3 fs, vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness){
	vec3 H = normalize(V + L);

	float NoV = abs(dot(N, V)) + 0.00001; // avoid artifact
	float LoH = saturate(dot(L, H));
	float NoH = saturate(dot(N, H));
	float NoL = saturate(dot(N, L));

	float roughness2 = roughness * roughness; // squaring remapping

	fd = (1 - metallic) * DisneyDiffuseFr(NoV, NoL, LoH, roughness) * INV_PI * albedo;

	vec3 F = SchlickFr(LoH, albedo, metallic);
	float Vis = SchlickGGX_SmithG_Vis_HC(NoV, NoL, roughness2);
	float D = SchlickGGX_D(NoH, roughness2);
	fs = D * F * Vis * INV_PI;
}

vec3 BRDF_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness) {
	vec3 fd, fs;
	BRDF_Frostbite(fd, fs, N, V, L, albedo, metallic, roughness);
	return fd + fs;
}

vec3 BRDFd_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness) {
	return (1 - metallic) * DisneyDiffuseFr(N, V, L, roughness) * INV_PI * albedo;
}

vec3 BRDFs_Frostbite(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness) {
	vec3 H = normalize(V + L);

	float NoV = abs(dot(N, V)) + 0.00001; // avoid artifact
	float LoH = saturate(dot(L, H));
	float NoH = saturate(dot(N, H));
	float NoL = saturate(dot(N, L));

	float roughness2 = roughness * roughness; // squaring remapping

	vec3 F = SchlickFr(LoH, albedo, metallic);
	float Vis = SchlickGGX_SmithG_Vis_HC(NoV, NoL, roughness2);
	float D = SchlickGGX_D(NoH, roughness2);

	return D * F * Vis * INV_PI;
}

vec3 Frostbite_SpecularDominantDir(vec3 N, vec3 R, float roughness) {
    float smoothness = clamp(1 - roughness, 0, 1);
    float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
    // The result is not normalized as we fetch in a cubemap
    return mix(N, R, lerpFactor);
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
