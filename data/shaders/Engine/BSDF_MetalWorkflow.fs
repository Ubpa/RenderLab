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
#define MAX_SPOT_LIGHTS 8
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
// 32
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float radius;	// 4	28
};

// 96
struct DirectionalLight{
	vec3 L;         // 12   0
	vec3 dir;       // 12   16
	mat4 ProjView;  // 64   32
};

// 128
struct SpotLight{
    vec3 position;         // 12     0
	vec3 dir;              // 12    16
    vec3 L;                // 12    32
	mat4 ProjView;         // 64    48
    float radius;          // 4     116
	float cosHalfAngle;    // 4     120
	float cosFalloffAngle; // 4     124
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

// 272
layout (std140) uniform PointLights{
	int numPointLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 32 * MAX_POINT_LIGHTS = 32 * 8 = 256
};

// 784
layout (std140) uniform DirectionalLights{
	int numDirectionalLight;// 16
	DirectionalLight directionaLights[MAX_DIRECTIONAL_LIGHTS];// 96 * MAX_DIRECTIONAL_LIGHTS = 96 * 8 = 768
};

// 1040
layout (std140) uniform SpotLights{
	int numSpotLight;// 16
	SpotLight spotLights[MAX_SPOT_LIGHTS];// 128 * MAX_SPOT_LIGHTS = 128 * 8 = 1024
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

uniform sampler2D directionalLightDepthMap0; // 9
uniform sampler2D directionalLightDepthMap1;
uniform sampler2D directionalLightDepthMap2;
uniform sampler2D directionalLightDepthMap3;
uniform sampler2D directionalLightDepthMap4;
uniform sampler2D directionalLightDepthMap5;
uniform sampler2D directionalLightDepthMap6;
uniform sampler2D directionalLightDepthMap7;

uniform sampler2D spotLightDepthMap0; // 17
uniform sampler2D spotLightDepthMap1;
uniform sampler2D spotLightDepthMap2;
uniform sampler2D spotLightDepthMap3;
uniform sampler2D spotLightDepthMap4;
uniform sampler2D spotLightDepthMap5;
uniform sampler2D spotLightDepthMap6;
uniform sampler2D spotLightDepthMap7;

uniform samplerCube skybox; // 25
uniform samplerCube irradianceMap; // 26
uniform samplerCube prefilterMap; // 27
uniform sampler2D   brdfLUT; // 28

uniform float lightNear;
uniform float lightFar;

// ----------------- 函数声明

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

float SchlickGGX_D(vec3 norm, vec3 h, float roughness);
float SchlickGGX_G1(vec3 norm, vec3 w, float roughness);
float SchlickGGX_G(vec3 norm, vec3 wo, vec3 wi, float roughness);
vec3 GetF0(vec3 albedo, float metallic);
vec3 Fr(vec3 w, vec3 h, vec3 albedo, float metallic);
vec3 FrR(vec3 wo, vec3 norm, vec3 F0, float roughness);

float Fwin(float d, float radius);

vec3 BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);

float PointLightVisibility(vec3 lightToFrag, int id);
float PointLightVisibility(vec3 lightToFrag, samplerCube depthMap);

float DirectionalLightVisibility(vec3 normPos, float cosTheta, int id);
float DirectionalLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);

float SpotLightVisibility(vec3 normPos, float cosTheta, int id);
float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);
float SpotLightDirFalloff(vec3 wi, int id);
float LinearizeDepth(float depth, float near, float far) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}
float PerpDepth(float linearDepth, float near, float far) {
	float z = (near + far - 2.0 * near * far / linearDepth) / (far-near);
	return (z + 1.0) / 2.0;
}

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
	
	// point light
    for(int i = 0; i < numLight; i++) {
		vec3 fragToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		
		float falloff = Fwin(dist, pointLights[i].radius);
		if(falloff < 0.000001)
			continue;
		
		float visibility = PointLightVisibility(-fragToLight, i);
		
		float attenuation = max(0.0001, dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = max(dot(wi, norm), 0);

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += visibility * cosTheta / attenuation * falloff * pointLights[i].L * f;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++){
		vec3 wi = -normalize(directionaLights[i].dir);

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = directionaLights[i].ProjView * vec4(fs_in.FragPos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = DirectionalLightVisibility(normPos, cosTheta, i);
		
		result += visibility * cosTheta * f * directionaLights[i].L;
	}
	
	// spot light
	for(int i=0; i < numSpotLight; i++){
		vec3 fragToLight = spotLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;
		
		float distFalloff = Fwin(dist, spotLights[i].radius);
		float dirFalloff = SpotLightDirFalloff(wi, i);
		float falloff = dirFalloff * distFalloff;
		if(falloff < 0.000001)
			continue;
		
		float attenuation = max(0.0001, dist2);

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = spotLights[i].ProjView * vec4(fs_in.FragPos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);

		vec3 f = BRDF(norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += visibility * cosTheta / attenuation * falloff * spotLights[i].L * f;
	}
	
	// ambient light
	if(haveEnvironment) {
		vec3 F0 = GetF0(albedo, metallic);
		vec3 F = FrR(wo, norm, F0, roughness);
		vec3 kS = F;
		vec3 kD = (1 - metallic) * (vec3(1) - kS);
		
		vec3 irradiance = haveSkybox ? texture(irradianceMap, norm).rgb : vec3(1);
		
		vec3 diffuse = irradiance * albedo;
		
		vec3 R = reflect(-wo, norm);
		const float MAX_REFLECTION_LOD = 4.0;
		// 用 R 来采样
		vec3 prefilteredColor = haveSkybox ? textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb : vec3(1);
		vec2 envBRDF = texture(brdfLUT, vec2(max(dot(norm, wo), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);
		
		vec3 ambient = (kD * diffuse + specular) * ao * intensity * colorFactor;
		result += ambient;
	}
	
	// gamma 校正
	result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}

// ----------------- 函数定义

float SchlickGGX_D(vec3 norm, vec3 h, float roughness){
	float NoH = dot(norm, h);
	if(NoH < 0)
		return 0;
	
	float alpha = roughness * roughness;
	
	float alpha2 = alpha * alpha;
	float cos2Theta = NoH * NoH;
	
	float t = (alpha2 - 1) * cos2Theta + 1;
	
	return alpha2 / (PI * t * t);
}

float SchlickGGX_G1(vec3 norm, vec3 w, float roughness) {
	float k = (roughness+1) * (roughness+1) / 8;
	
	float NoW = max(0.f, dot(norm, w));
	return NoW / (NoW * (1 - k) + k);
}

float SchlickGGX_G(vec3 norm, vec3 wo, vec3 wi, float roughness){
	return SchlickGGX_G1(norm, wo, roughness) * SchlickGGX_G1(norm, wi, roughness);
}

vec3 GetF0(vec3 albedo, float metallic) {
    return mix(vec3(0.04), albedo, metallic);
}

vec3 Fr(vec3 w, vec3 h, vec3 albedo, float metallic) {
	vec3 F0 = GetF0(albedo, metallic);
	float HoW = dot(h, w);
	return F0 + exp2((-5.55473f * HoW - 6.98316f) * HoW) * (vec3(1.0f) - F0);
}

vec3 FrR(vec3 wo, vec3 norm, vec3 F0, float roughness) {
	float cosTheta = max(dot(wo, norm), 0);
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 BRDF(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao) {
	vec3 wh = normalize(wo + wi);
	
	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_G(norm, wo, wi, roughness);
	vec3 F = Fr(wo, wh, albedo, metallic);
	
	vec3 specular = D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
	
	vec3 diffuse = albedo * INV_PI;
	
	vec3 kS = 1 - F;
	vec3 kD = (1-metallic) * (1 - kS);
	
	vec3 rst = kD * diffuse + specular;

	//return ao * rst;
	return rst;
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

float Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = max(0, 1 - ratio4);
	return falloff * falloff;
}

float PointLightVisibility(vec3 lightToFrag, int id){
	if(id == 0) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap0);
	} else if(id == 1) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap1);
	} else if(id == 2) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap2);
	} else if(id == 3) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap3);
	} else if(id == 4) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap4);
	} else if(id == 5) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap5);
	} else if(id == 6) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap6);
	} else if(id == 7) {
		return PointLightVisibility(lightToFrag, pointLightDepthMap7);
	}else 
		return 1;// not support id
}

float PointLightVisibility(vec3 lightToFrag, samplerCube depthMap) {
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

float DirectionalLightVisibility(vec3 normPos, float cosTheta, int id){
	if(id == 0) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap0);
	} else if(id == 1) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap1);
	} else if(id == 2) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap2);
	} else if(id == 3) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap3);
	} else if(id == 4) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap4);
	} else if(id == 5) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap5);
	} else if(id == 6) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap6);
	} else if(id == 7) {
		return DirectionalLightVisibility(normPos, cosTheta, directionalLightDepthMap7);
	}else
		return 1;// not support id
}

float DirectionalLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap){
	float visibility = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	float bias = max(0.05 * (1.0 - cosTheta), 0.005);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, normPos.xy + vec2(x, y) * texelSize).r; 
			visibility += smoothstep(normPos.z - bias, normPos.z - 0.5 * bias, pcfDepth);      
		}
	}
	visibility /= 9.0;
	return visibility;
}

float SpotLightVisibility(vec3 normPos, float cosTheta, int id){
	if(id == 0) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap0);
	} else if(id == 1) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap1);
	} else if(id == 2) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap2);
	} else if(id == 3) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap3);
	} else if(id == 4) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap4);
	} else if(id == 5) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap5);
	} else if(id == 6) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap6);
	} else if(id == 7) {
		return SpotLightVisibility(normPos, cosTheta, spotLightDepthMap7);
	}else
		return 1;// not support id
}

float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap){
	float visibility = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	float bias = max(0.05 * (1.0 - cosTheta), 0.005);
	float linearDepth = LinearizeDepth(normPos.z, lightNear, lightFar);
	float moveZ = PerpDepth(linearDepth - bias * (lightFar - lightNear), lightNear, lightFar);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap, normPos.xy + vec2(x, y) * texelSize).r;
			visibility += smoothstep(moveZ, normPos.z, pcfDepth);
		}
	}
	visibility /= 9.0;
	return visibility;
}

float SpotLightDirFalloff(vec3 wi, int id){
	float cosTheta = -dot(wi, spotLights[id].dir);
	if (cosTheta < spotLights[id].cosHalfAngle) return 0;
	if (cosTheta > spotLights[id].cosFalloffAngle) return 1;

	float delta = (cosTheta - spotLights[id].cosHalfAngle) /
		(spotLights[id].cosHalfAngle - spotLights[id].cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
