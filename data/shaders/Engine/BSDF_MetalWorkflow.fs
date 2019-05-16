#version 330 core

// ----------------- 输入输出

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

// ----------------- 常量

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 8
const float PI = 3.14159265359;
const float INV_PI = 0.31830988618;

// array of offset direction for sampling
const vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// ----------------- 结构

struct BSDF_MetalWorkflow {
	vec3 colorFactor;
	bool haveAlbedoTexture;
    sampler2D albedoTexture;

	float metallicFactor;
	bool haveMetallicTexture;
    sampler2D metallicTexture;

	float roughnessFactor;
	bool haveRoughnessTexture;
    sampler2D roughnessTexture;
	
	bool haveAOTexture;
    sampler2D aoTexture;

	bool haveNormalTexture;
	sampler2D normalTexture;
};

// ----------------- Uniform

// 48
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float linear;	// 4	28
    float quadratic;// 4	32
};

// 32
struct DirectionalLight{
	vec3 L;         // 12   0
	vec3 dir;       // 12   16
};

// 160
layout (std140) uniform Camera{
	mat4 view;			// 64	0	64
	mat4 projection;	// 64	64	64
	vec3 viewPos;		// 12	128	144
	float nearPlane;	// 4	144	148
	float farPlane;		// 4	148	152
	float fov;			// 4	152	156
	float ar;			// 4	156	160
};

// 400
layout (std140) uniform PointLights{
	int numLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 8
};

// 272
layout (std140) uniform DirectionalLights{
	int numDirectionalLight;// 16
	DirectionalLight directionaLights[MAX_DIRECTIONAL_LIGHTS];// 32 * MAX_DIRECTIONAL_LIGHTS = 32 * 8 = 256
};

uniform BSDF_MetalWorkflow bsdf;

uniform samplerCube pointLightDepthMap0;
uniform samplerCube pointLightDepthMap1;
uniform samplerCube pointLightDepthMap2;
uniform samplerCube pointLightDepthMap3;
uniform samplerCube pointLightDepthMap4;
uniform samplerCube pointLightDepthMap5;
uniform samplerCube pointLightDepthMap6;
uniform samplerCube pointLightDepthMap7;

uniform sampler2D directionaLightDepthMap0; // 9
uniform sampler2D directionaLightDepthMap1;
uniform sampler2D directionaLightDepthMap2;
uniform sampler2D directionaLightDepthMap3;
uniform sampler2D directionaLightDepthMap4;
uniform sampler2D directionaLightDepthMap5;
uniform sampler2D directionaLightDepthMap6;
uniform sampler2D directionaLightDepthMap7;

uniform float lightFar;

// ----------------- 函数声明

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

float NDF(vec3 norm, vec3 h, float roughness);
vec3 Fr(vec3 wi, vec3 h, vec3 albedo, float metallic);
float G(vec3 norm, vec3 wo, vec3 wi, float roughness);
vec3 MS_BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 fr, vec3 albedo, float roughness);
vec3 BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);

float Visibility(vec3 lightToFrag, int id);
float Visibility(vec3 lightToFrag, samplerCube depthMap);

// ----------------- 主函数

void main() {
	// 获取属性值
	vec3 albedo = bsdf.colorFactor;
	if(bsdf.haveAlbedoTexture) {
		albedo *= texture(bsdf.albedoTexture, fs_in.TexCoords).xyz;
	}

	float metallic = bsdf.metallicFactor;
	if(bsdf.haveMetallicTexture) {
		metallic *= texture(bsdf.metallicTexture, fs_in.TexCoords).x;
	}

	float roughness = bsdf.roughnessFactor;
	if(bsdf.haveRoughnessTexture) {
		roughness *= texture(bsdf.roughnessTexture, fs_in.TexCoords).x;
	}

	float ao = 1.0f;
	if(bsdf.haveAOTexture) {
		ao *= texture(bsdf.aoTexture, fs_in.TexCoords).x;
	}

	vec3 wo = normalize(viewPos - fs_in.FragPos);

	vec3 norm = normalize(fs_in.Normal);
	if(bsdf.haveNormalTexture) {
		norm = CalcBumpedNormal(norm, normalize(fs_in.Tangent), bsdf.normalTexture, fs_in.TexCoords);
	}
	
	// 采样光源
	vec3 result = vec3(0);
    for(int i = 0; i < numLight; i++) {
		vec3 fragToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight / dist;
		
		float visibility = Visibility(-fragToLight, i);
		if(visibility==0)
			continue;

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += visibility * cosTheta / attenuation * f * pointLights[i].L;
	}
	
	for(int i=0; i < numDirectionalLight; i++){
		vec3 wi = -normalize(directionaLights[i].dir);

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
		result += cosTheta * f * directionaLights[i].L;
	}
	
	// gamma 校正
    FragColor = vec4(sqrt(result), 1.0);
}

// ----------------- 函数定义

float NDF(vec3 norm, vec3 h, float roughness) {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NoH = dot(norm, h);
	return alpha2 / (PI*pow(NoH*NoH*(alpha2 - 1) + 1, 2));
}

vec3 Fr(vec3 wi, vec3 h, vec3 albedo, float metallic) {
	vec3 F0 = mix(vec3(0.04f), albedo, metallic);
	float HoWi = dot(h, wi);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (vec3(1.0f) - F0);
}

float G(vec3 norm, vec3 wo, vec3 wi, float roughness) {
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);

	float k = pow(roughness + 1, 2) / 8.f;
	float one_minus_k = 1 - k;

	float G1_wo = max(NoWo, 0) / (NoWo * one_minus_k + k);
	float G1_wi = max(NoWi, 0) / (NoWi * one_minus_k + k);

	return G1_wo * G1_wi;
}

vec3 MS_BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 fr, vec3 albedo, float roughness) {
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);
	vec3 h = normalize(wo + wi);
	return NDF(norm, h, roughness) * G(norm, wo, wi, roughness) / (4 * NoWo * NoWi) * fr;
}

vec3 BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao) {
	vec3 h = normalize(wo + wi);
	vec3 diffuse = albedo * INV_PI;
	vec3 fr = Fr(wi, h, albedo, metallic);

	return ao * ((1 - metallic)*(1.0f - fr)*diffuse + MS_BRDF(norm, wo, wi, fr, albedo, roughness));
}

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord) {
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 bumpMapNormal = texture(normalTexture, texcoord).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - 1.0;
    mat3 TBN = mat3(tangent, bitangent, normal);
    vec3 newNormal = TBN * bumpMapNormal;
    newNormal = normalize(newNormal);
    return newNormal;
}

float Visibility(vec3 lightToFrag, int id){
	if(id == 0) {
		return Visibility(lightToFrag, pointLightDepthMap0);
	} else if(id == 1) {
		return Visibility(lightToFrag, pointLightDepthMap1);
	} else if(id == 2) {
		return Visibility(lightToFrag, pointLightDepthMap2);
	} else if(id == 3) {
		return Visibility(lightToFrag, pointLightDepthMap3);
	} else if(id == 4) {
		return Visibility(lightToFrag, pointLightDepthMap4);
	} else if(id == 5) {
		return Visibility(lightToFrag, pointLightDepthMap5);
	} else if(id == 6) {
		return Visibility(lightToFrag, pointLightDepthMap6);
	} else if(id == 7) {
		return Visibility(lightToFrag, pointLightDepthMap7);
	}else 
		return 1;// not support id
}

float Visibility(vec3 lightToFrag, samplerCube depthMap) {
	float currentDepth = length(lightToFrag);
	float bias = 0.08;
	int samples = 20;
	float shadow = 0.0;
	float viewDistance = length(viewPos - fs_in.FragPos);
	float diskRadius = (1.0 + (viewDistance / lightFar)) / 50.0;
	for(int i = 0; i < samples; ++i) {
		float closestDepth = lightFar * texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
		shadow += smoothstep(closestDepth, closestDepth + bias, currentDepth);
	}
	shadow /= float(samples);
	return 1 - shadow;
}
