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

struct BSDF_FrostedGlass {
	vec3 colorFactor;
	bool haveColorTexture;
    sampler2D colorTexture;

	float roughnessFactor;
	bool haveRoughnessTexture;
    sampler2D roughnessTexture;
	
	bool haveAOTexture;
    sampler2D aoTexture;

	bool haveNormalTexture;
	sampler2D normalTexture;

	float ior;
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

uniform BSDF_FrostedGlass bsdf;

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

uniform float lightNear;
uniform float lightFar;

// ----------------- 函数声明

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

float GGX_D(vec3 norm, vec3 h, float alpha);
float Fr(vec3 v, vec3 h, float ior);
float GGX_G1(vec3 norm, vec3 v, vec3 h, float alpha);
float GGX_G(vec3 norm, vec3 wo, vec3 wi, vec3 h, float alpha);
vec3 BSDF(vec3 norm, vec3 wo, vec3 wi, vec3 color, float roughness, float ao, float ior);

float Fwin(float d, float radius);

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
	vec3 color = bsdf.colorFactor;
	if(bsdf.haveColorTexture) {
		color *= texture(bsdf.colorTexture, fs_in.TexCoords).xyz;
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

		vec3 f = BSDF(norm, wo, wi, color, roughness, ao, bsdf.ior);
		
		result += visibility * cosTheta / attenuation * falloff * pointLights[i].L * f;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++){
		vec3 wi = -normalize(directionaLights[i].dir);

		vec3 f = BSDF(norm, wo, wi, color, roughness, ao, bsdf.ior);

		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
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
		
		float distFalloff = Fwin(dist, spotLights[i].radius);
		float dirFalloff = SpotLightDirFalloff(wi, i);
		float falloff = dirFalloff * distFalloff;
		if(falloff < 0.000001)
			continue;
		
		float attenuation = max(0.0001, dist2);

		vec3 wi = fragToLight/dist;
		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
		vec4 pos4 = spotLights[i].ProjView * vec4(fs_in.FragPos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);

		vec3 f = BSDF(norm, wo, wi, color, roughness, ao, bsdf.ior);
		
		result += visibility * cosTheta / attenuation * falloff * spotLights[i].L * f;
	}
	
	// gamma 校正
    FragColor = vec4(sqrt(result), 1.0);
}

// ----------------- 函数定义

float GGX_D(vec3 norm, vec3 h, float alpha) {
	float HoN = dot(h, norm);
	if (HoN <= 0)
		return 0;

	float cos2 = HoN * HoN;
	float alpha2 = alpha * alpha;

	float root = alpha / (cos2 * (alpha2 - 1) + 1);

	return INV_PI * root * root;
}

float Fr(vec3 v, vec3 h, float ior) {
	// angle in low ior media
	float cosTheta = dot(v, h);
	bool entering = cosTheta > 0.0f;
	
	if (!entering) {
		vec3 vInLowIOR = refract(-v, -h, ior);
		if (vInLowIOR == vec3(0))
			return 1;

		cosTheta = dot(vInLowIOR, h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	float Fr = R0 + (1 - R0) * pow((1 - cosTheta), 5);

	return Fr;
}

float GGX_G1(vec3 norm, vec3 v, vec3 h, float alpha) {
	float NoV = dot(norm, v);
	float HoV = dot(v, h);
	if (NoV * HoV <= 0)
		return 0;

	float alpha2 = alpha * alpha;
	float tan2 = 1.f / (NoV * NoV) - 1.f;
	return 2.f / (1.f + sqrt(1 + alpha2 * tan2));
}

float GGX_G(vec3 norm, vec3 wo, vec3 wi, vec3 h, float alpha) {
	return GGX_G1(norm, wo, h, alpha) * GGX_G1(norm, wi, h, alpha);
}

vec3 BSDF(vec3 norm, vec3 wo, vec3 wi, vec3 color, float roughness, float ao, float ior) {
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);
	if(NoWo == 0 || NoWi == 0)
		return vec3(0);

	bool entering = NoWo > 0;
	bool isReflect = NoWo * NoWi > 0;

	float alpha = roughness * roughness;

	if (isReflect) {
		vec3 h = normalize(wo + wi);

		h *= sign(dot(h, norm));// let h point to outside
		float bsdfVal = Fr(wo, h, ior) * GGX_D(norm, h, alpha) * GGX_G(norm, wo, wi, h, alpha) / abs(4.f * NoWo * NoWi);
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!entering){
			// swap(etai, etat);
			float tmp = etai;
			etai = etat;
			etat = tmp;
		}

		vec3 h = -normalize(etai * wo + etat * wi);

		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (NoWo * NoWi));

		h *= sign(dot(h,norm));// let h point to outside
		float bsdfVal = factor * ((1 - Fr(wo, h, ior)) * GGX_D(norm, h, alpha) * GGX_G(norm, wo, wi, h, alpha) * etat * etat) / (sqrtDenom * sqrtDenom);
		return bsdfVal * color;
	}
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
		if(closestDepth < currentDepth - bias)
			shadow += 1;
		//shadow += smoothstep(closestDepth, closestDepth + bias, currentDepth);
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
