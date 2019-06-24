#version 330 core

// ----------------- 输入输出

out vec3 FragColor;

in vec2 TexCoords;

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

// ----------------- Uniform

// 48
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float linear;	// 4	28
    float quadratic;// 4	32
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
    float linear;	       // 4     112
    float quadratic;       // 4     116
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

// 400
layout (std140) uniform PointLights{
	int numPointLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 8
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

uniform sampler2D GBuffer0;
uniform sampler2D GBuffer1;
uniform sampler2D GBuffer2;
uniform sampler2D GBuffer3;

uniform samplerCube pointLightDepthMap0; // 4
uniform samplerCube pointLightDepthMap1;
uniform samplerCube pointLightDepthMap2;
uniform samplerCube pointLightDepthMap3;
uniform samplerCube pointLightDepthMap4;
uniform samplerCube pointLightDepthMap5;
uniform samplerCube pointLightDepthMap6;
uniform samplerCube pointLightDepthMap7;

uniform sampler2D directionalLightDepthMap0; // 12
uniform sampler2D directionalLightDepthMap1;
uniform sampler2D directionalLightDepthMap2;
uniform sampler2D directionalLightDepthMap3;
uniform sampler2D directionalLightDepthMap4;
uniform sampler2D directionalLightDepthMap5;
uniform sampler2D directionalLightDepthMap6;
uniform sampler2D directionalLightDepthMap7;

uniform sampler2D spotLightDepthMap0; // 20
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

float SchlickGGX_D(vec3 norm, vec3 h, float roughness);
float SchlickGGX_G1(vec3 norm, vec3 w, float roughness);
float SchlickGGX_G(vec3 norm, vec3 wo, vec3 wi, float roughness);
vec3 GetF0(vec3 albedo, float metallic);
vec3 SchlickFr(vec3 w, vec3 h, vec3 albedo, float metallic);
vec3 SchlickFrR(vec3 wo, vec3 norm, vec3 F0, float roughness);
float DisneyDiffuseFr(vec3 norm, vec3 wo, vec3 wi, float linearRoughness);

vec3 BRDF(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);
vec3 BRDF_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);
vec3 BRDF_Diffuse(vec3 albedo);
vec3 BRDF_Frostbite(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao);

float PointLightVisibility(vec3 lightToFrag, int id);
float PointLightVisibility(vec3 lightToFrag, samplerCube depthMap);

float DirectionalLightVisibility(vec3 normPos, float cosTheta, int id);
float DirectionalLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);

float SpotLightVisibility(vec3 normPos, float cosTheta, int id);
float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);
float SpotLightFalloff(vec3 wi, int id);
float LinearizeDepth(float depth, float near, float far) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}
float PerpDepth(float linearDepth, float near, float far) {
	float z = (near + far - 2.0 * near * far / linearDepth) / (far-near);
	return (z + 1.0) / 2.0;
}

vec3 pos;

// ----------------- 主函数

void main() {
	// 获取属性值
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
	
	// 采样光源
	
	// point light
	vec3 result = vec3(0);
    for(int i = 0; i < numPointLight; i++) {
		vec3 fragToLight = pointLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight / dist;
		
		float visibility = PointLightVisibility(-fragToLight, i);
		if(visibility==0)
			continue;

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += visibility * cosTheta / attenuation * f * pointLights[i].L;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++) {
		vec3 wi = -normalize(directionaLights[i].dir);

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = directionaLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = DirectionalLightVisibility(normPos, cosTheta, i);
		
		result += visibility * cosTheta * f * directionaLights[i].L;
	}
	
	// spot light
	for(int i=0; i < numSpotLight; i++) {
		vec3 fragToLight = spotLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + spotLights[i].linear * dist + spotLights[i].quadratic * dist2;
		
		vec4 pos4 = spotLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);
		
		result += visibility * SpotLightFalloff(wi, i) * cosTheta / attenuation * f * spotLights[i].L;
	}
	
    FragColor = result;
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

vec3 SchlickFr(vec3 w, vec3 h, vec3 albedo, float metallic) {
	vec3 F0 = GetF0(albedo, metallic);
	float HoW = dot(h, w);
	return F0 + exp2((-5.55473f * HoW - 6.98316f) * HoW) * (vec3(1.0f) - F0);
}

vec3 SchlickFrR(vec3 wo, vec3 norm, vec3 F0, float roughness) {
	float cosTheta = max(dot(wo, norm), 0);
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DisneyDiffuseFr(vec3 norm, vec3 wo, vec3 wi, float linearRoughness) {
	vec3 h = normalize(wo + wi);
	float HoWi = dot(h, wi);
	float HoWi2 = HoWi * HoWi;
	
	float NoWo = dot(norm, wo);
	float NoWi = dot(norm, wi);
	
	float energyBias = mix(0.f, 0.5f, linearRoughness);
	float energyFactor = mix(1.f, 1.f / 1.51f, linearRoughness);
	float fd90 = energyBias + 2.f * HoWi2 * linearRoughness;
	float lightScatter = 1.f + fd90 * pow(1.f - NoWi * NoWi, 5);
	float viewScatter = 1.f + fd90 * pow(1.f - NoWo * NoWo, 5);
	
	return lightScatter * viewScatter * energyFactor;
}

vec3 BRDF(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao) {
	if(ID == 0) {
		return BRDF_MetalWorkflow(norm, wo, wi, albedo, metallic, roughness, ao);
	}
	else if(ID == 1) {
		return BRDF_Diffuse(albedo);
	}
	else if(ID == 2) {
		return BRDF_Frostbite(norm, wo, wi, albedo, metallic, roughness, ao);
	}
	else
		return vec3(0);// not support ID
}

vec3 BRDF_MetalWorkflow(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao) {
	vec3 wh = normalize(wo + wi);
	
	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_G(norm, wo, wi, roughness);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);
	
	vec3 specular = D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
	
	vec3 diffuse = albedo * INV_PI;
	
	vec3 kS = 1 - F;
	vec3 kD = (1-metallic) * (1 - kS);
	
	vec3 rst = kD * diffuse + specular;
	
	return rst;
}

vec3 BRDF_Diffuse(vec3 albedo) {
	return albedo / PI;
}

vec3 BRDF_Frostbite(vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness, float ao) {
	vec3 wh = normalize(wo + wi);
	
	float D = SchlickGGX_D(norm, wh, roughness);
	float G = SchlickGGX_G(norm, wo, wi, roughness);
	vec3 F = SchlickFr(wo, wh, albedo, metallic);
	
	vec3 specular = D * G * F / (4.0f * dot(wh, wo) * dot(wh, wi));
	
	vec3 diffuse = albedo * INV_PI * DisneyDiffuseFr(norm, wo, wi, roughness);
	
	vec3 rst = (1 - metallic) * diffuse + specular;
	
	return rst;
}

// ------------------------ Visibility ------------------------ 

float PointLightVisibility(vec3 lightToFrag, int id) {
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
	float viewDistance = length(viewPos - pos);
	float diskRadius = (1.0 + (viewDistance / lightFar)) / 50.0;
	for(int i = 0; i < samples; ++i) {
		float closestDepth = lightFar * texture(depthMap, lightToFrag + gridSamplingDisk[i] * diskRadius).r;
		shadow += smoothstep(closestDepth, closestDepth + bias, currentDepth);
	}
	shadow /= float(samples);
	return 1 - shadow;
}

float DirectionalLightVisibility(vec3 normPos, float cosTheta, int id) {
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

float DirectionalLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap) {
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

float SpotLightVisibility(vec3 normPos, float cosTheta, int id) {
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

float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap) {
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

float SpotLightFalloff(vec3 wi, int id) {
	float cosTheta = -dot(wi, spotLights[id].dir);
	if (cosTheta < spotLights[id].cosHalfAngle) return 0;
	if (cosTheta > spotLights[id].cosFalloffAngle) return 1;

	float delta = (cosTheta - spotLights[id].cosHalfAngle) /
		(spotLights[id].cosHalfAngle - spotLights[id].cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
