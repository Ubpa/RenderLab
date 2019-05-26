#version 330 core

layout (location = 0) out vec3 Normal;
layout (location = 1) out vec3 Albedo;
layout (location = 2) out vec3 RMAO;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

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

uniform BSDF_MetalWorkflow metal;

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

void main(){
	// 获取属性值
	vec3 albedo = metal.colorFactor;
	if(metal.haveAlbedoTexture) {
		albedo *= texture(metal.albedoTexture, fs_in.TexCoords).xyz;
	}

	float metallic = metal.metallicFactor;
	if(metal.haveMetallicTexture) {
		metallic *= texture(metal.metallicTexture, fs_in.TexCoords).x;
	}

	float roughness = metal.roughnessFactor;
	if(metal.haveRoughnessTexture) {
		roughness *= texture(metal.roughnessTexture, fs_in.TexCoords).x;
	}

	float ao = 1.0f;
	if(metal.haveAOTexture) {
		ao *= texture(metal.aoTexture, fs_in.TexCoords).x;
	}

	vec3 norm = normalize(fs_in.Normal);
	if(metal.haveNormalTexture) {
		norm = CalcBumpedNormal(norm, normalize(fs_in.Tangent), metal.normalTexture, fs_in.TexCoords);
	}
	
	Normal = norm;
	Albedo = albedo;
	RMAO = vec3(roughness, metallic, ao);
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
