#ifndef BRDF_METAL_WORKFLOW_H
#define BRDF_METAL_WORKFLOW_H

#include "FDG.h"

// ------------------------------ 接口 ------------------------------

void BRDF_MetalWorkflow(out vec3 fd, out vec3 fs, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
vec3 BRDF_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

// diffuse
vec3 BRDFd_MetalWorkflow(vec3 wo, vec3 wi, vec3 albedo, float metallic);
// specular
vec3 BRDFs_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

// ------------------------------ 实现 ------------------------------

void BRDF_MetalWorkflow(out vec3 fd, out vec3 fs, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	vec3 wh = normalize(wo + wi);
	
	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_SmithG_Analytic(norm, wo, wi, roughness);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);
	
	vec3 kS = F;
	vec3 kD = (1-metallic) * (1 - kS);
	
	fd = kD * albedo * INV_PI;
	fs = D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
}

vec3 BRDF_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness){
	vec3 fd, fs;
	BRDF_MetalWorkflow(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	return fd + fs;
}

// diffuse
vec3 BRDFd_MetalWorkflow(vec3 wo, vec3 wi, vec3 albedo, float metallic) {
	vec3 wh = normalize(wo + wi);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);
	vec3 kD = (1 - metallic) * (1 - F);
	return kD * albedo * INV_PI;
}

// specular
vec3 BRDFs_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	vec3 wh = normalize(wo + wi);

	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_SmithG_Analytic(norm, wo, wi, roughness);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);

	return D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
}

#endif // !BRDF_METAL_WORKFLOW_H
