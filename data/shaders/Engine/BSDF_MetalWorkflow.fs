#version 330 core
out vec4 FragColor;

#define MAX_POINT_LIGHTS 16

struct BSDF_MetalWorkflow {
	vec3 albedoColor;
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

// 48
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float linear;	// 4	28
    float quadratic;// 4	32
};

const float PI = 3.14159265359;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

// 144
layout (std140) uniform Camera{
	mat4 view;			// 64	0
	mat4 projection;	// 64	64
	vec3 viewPos;		// 12	128
};

// 784
layout (std140) uniform PointLights{
	int numLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 32
};

uniform BSDF_MetalWorkflow bsdf;

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);
float NDF(vec3 norm, vec3 h, float roughness);
vec3 Fr(vec3 wi, vec3 h, vec3 albedo, float metallic);
float G(vec3 norm, vec3 wo, vec3 wi, float roughness);
vec3 MS_BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 fr, vec3 albedo, float roughness);
vec3 BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);

void main()
{
	vec3 albedo = bsdf.albedoColor;
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
	
	vec3 result = vec3(0);
    for(int i = 0; i < numLight; i++) {
		vec3 dirToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(dirToLight, dirToLight);
		float dist = sqrt(dist2);
		vec3 wi = dirToLight / dist;

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += cosTheta / attenuation * f * pointLights[i].L;
	}
	
    FragColor = vec4(sqrt(result), 1.0);
}

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
	vec3 diffuse = albedo / PI;
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
