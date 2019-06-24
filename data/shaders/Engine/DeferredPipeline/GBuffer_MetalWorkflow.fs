#version 330 core

layout (location = 0) out vec4 GBuffer0;
layout (location = 1) out vec4 GBuffer1;
layout (location = 2) out vec4 GBuffer2;
layout (location = 3) out vec4 GBuffer3;

// layout
//      x   y   z   w
// 0 [    pos   ]  ID
// 1 [   norm   ]   r
// 2 [  albedo  ]   m
// 3               ao

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
uniform int ID;

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

void main() {
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
	
	// pack GBuffer
	GBuffer0 = vec4(fs_in.FragPos, ID);
	GBuffer1 = vec4(norm, roughness);
	GBuffer2 = vec4(albedo, metallic);
	GBuffer3 = vec4(0, 0, 0, ao);
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
