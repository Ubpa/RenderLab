#version 330 core

#include "../BRDF/MetalWorkflow.h"
#include "../BRDF/Frostbite.h"
#include "../BRDF/Diffuse.h"

out vec3 FragColor;
in vec2 TexCoords;

#define MAX_REFLECTION_LOD 4

// 160
layout (std140) uniform Camera {
	mat4 view;			// 64	0	64
	mat4 projection;	// 64	64	64
	vec3 viewPos;		// 12	128	144
	float nearPlane;	// 4	144	148
	float farPlane;		// 4	148	152
	float fov;			// 4	152	156
	float ar;			// 4	156	160
};

// 32
layout (std140) uniform Environment {
	vec3 colorFactor;     // 12     0
	float intensity;      //  4    12
	bool haveSkybox;      //  4    16
	bool haveEnvironment; //  4    20
};

uniform sampler2D GBuffer0;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;
uniform sampler2D GBuffer3;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT;

vec3 EvaluateAmbient(int ID, vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao);
vec3 EvaluateAmbient_MetalWorkflow(vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao);
vec3 EvaluateAmbient_Diffuse(vec3 norm, vec3 albedo);
vec3 EvaluateAmbient_Frostbite(vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao);

void main() {
	if( !haveEnvironment ) {
		FragColor = vec3(0, 0, 0);
		return;
	}
	
	// prepare
	vec4 data0 = texture(GBuffer0, TexCoords);
	vec4 data1 = texture(GBuffer1, TexCoords);
	vec4 data2 = texture(GBuffer2, TexCoords);
	vec4 data3 = texture(GBuffer3, TexCoords);
	
	int ID = int(data0.w);
	vec3 pos = data0.xyz;
	vec3 norm = data1.xyz;
	vec3 albedo = data2.xyz;
	float roughness = data1.w;
	float metallic = data2.w;
	float ao = data3.w;
	
	vec3 wo = normalize(viewPos - pos);
	
	vec3 ambient = EvaluateAmbient(ID, wo, norm, albedo, roughness, metallic, ao);
	
	FragColor = ambient;
}

vec3 EvaluateAmbient(int ID, vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao) {
	if(ID == 0) {
		return EvaluateAmbient_MetalWorkflow(wo, norm, albedo, roughness, metallic, ao);
	}
	else if(ID == 1) {
		return EvaluateAmbient_Diffuse(norm, albedo);
	}
	else if(ID == 2) {
		return EvaluateAmbient_Frostbite(wo, norm, albedo, roughness, metallic, ao);
	}
	else
		return vec3(0); // not support ID
}

vec3 EvaluateAmbient_MetalWorkflow(vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao) {
	vec3 F0 = MetalWorkflowF0(albedo, metallic);
	vec3 F = SchlickFrR(wo, norm, F0, roughness);
	vec3 kS = F;
	vec3 kD = (1 - metallic) * (vec3(1) - kS);
	
	// diffuse
	vec3 irradiance = haveSkybox ? texture(irradianceMap, norm).rgb : vec3(1);
	vec3 diffuse = irradiance * albedo;
	
	// specular
	vec3 R = reflect(-wo, norm);
	// 用 R 采样
	vec3 prefilteredColor = haveSkybox ? textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb : vec3(1);
	float cosTheta = saturate(dot(norm, wo));
	vec2 envBRDF = texture(brdfLUT, vec2(cosTheta, roughness)).rg;
	vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
	
	return (kD * diffuse + specular) * ao * intensity * colorFactor;
}

vec3 EvaluateAmbient_Diffuse(vec3 norm, vec3 albedo) {
	vec3 irradiance = haveSkybox ? texture(irradianceMap, norm).rgb : vec3(1);
	return irradiance * albedo * intensity * colorFactor;
}

vec3 EvaluateAmbient_Frostbite(vec3 wo, vec3 norm, vec3 albedo, float roughness, float metallic, float ao) {
	float perpRoughness = roughness * roughness;
	float cosTheta = clamp(dot(norm, wo), 0, 1);
	vec3 envBRDF = texture(brdfLUT, vec2(cosTheta, perpRoughness)).rgb;
	
	// diffuse, need update to Disney Diffuse
	vec3 dominantN = Frostbite_DiffuseDominantDir(norm, wo, perpRoughness);
	vec3 irradiance = haveSkybox ? texture(irradianceMap, dominantN).rgb : vec3(1);
	vec3 diffuse = irradiance * envBRDF.z * albedo;
	
	// specular
	vec3 R = reflect(-wo, norm);
	vec3 dominantDir = Frostbite_SpecularDominantDir(norm, R, perpRoughness);
	// 用 dominantDir 采样
	vec3 prefilteredColor = haveSkybox ? textureLod(prefilterMap, dominantDir,  perpRoughness * MAX_REFLECTION_LOD).rgb : vec3(1);
	vec3 F0 = MetalWorkflowF0(albedo, metallic);
	vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
	
	return ((1 - metallic) * diffuse + specular) * ao * intensity * colorFactor;
}
