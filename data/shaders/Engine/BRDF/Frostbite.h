#ifndef BRDF_FROSTBITE_H
#define BRDF_FROSTBITE_H

#include "FDG.h"

// ------------------------------ 接口 ------------------------------

void BRDF_Frostbite(out vec3 fd, out vec3 fs, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
vec3 BRDF_Frostbite(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

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

#endif // !BRDF_FROSTBITE_H
