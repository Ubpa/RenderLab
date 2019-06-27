#version 330 core

// ----------------- 输入输出

out vec3 FragColor;

in vec2 TexCoords;

// ----------------- 常量

#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_SPOT_LIGHTS 8
#define MAX_SPHERE_LIGHTS 8
#define MAX_DISK_LIGHTS 8
#define MAX_AREA_LIGHTS 8
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

// 32
struct SphereLight {
    vec3 position;  // 12    0
    vec3 L;         // 12   16
    float radius;   //  4   28
};

// 48
struct DiskLight {
    vec3 position;  // 12    0
    vec3 dir;       // 12   32
    vec3 L;         // 12   16
    float radius;   //  4   44
};

// 64
struct AreaLight {
    vec3 position;  // 12    0
	float width;    // 12   12
    vec3 dir;       // 12   16
	float height;   //  4   28
	vec3 horizontal;// 12   32
    vec3 L;         // 12   48
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

// 272
layout (std140) uniform SphereLights{
	int numSphereLight;// 16
	SphereLight sphereLights[MAX_SPHERE_LIGHTS];// 32 * MAX_SPHERE_LIGHTS = 32 * 8 = 256
};

// 400
layout (std140) uniform DiskLights{
	int numDiskLight;// 16
	DiskLight diskLights[MAX_DISK_LIGHTS];// 48 * MAX_DISK_LIGHTS = 48 * 8 = 384
};

// 528
layout (std140) uniform AreaLights{
	int numAreaLight;// 16
	AreaLight areaLights[MAX_AREA_LIGHTS];// 64 * MAX_AREA_LIGHTS = 64 * 8 = 512
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
	
	if(ID == 3) { // emission
		FragColor = dot(norm, wo) > 0 ? albedo : vec3(0);
		return;
	}
	
	// for ID 0, 1, 2
	
	// 采样光源
	vec3 result = vec3(0);
	
	// point light
    for(int i = 0; i < numPointLight; i++) {
		vec3 fragToLight = pointLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		
		float falloff = Fwin(dist, pointLights[i].radius);
		if(falloff < 0.000001)
			continue;
		
		float visibility = PointLightVisibility(-fragToLight, i);
		
		float attenuation = max(0.0001, dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = max(dot(wi, norm), 0);

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += visibility * cosTheta / attenuation * falloff * pointLights[i].L * f;
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
		
		float distFalloff = Fwin(dist, spotLights[i].radius);
		float dirFalloff = SpotLightDirFalloff(wi, i);
		float falloff = dirFalloff * distFalloff;
		if(falloff < 0.000001)
			continue;
		
		float attenuation = max(0.0001, dist2);

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = spotLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += visibility * cosTheta / attenuation * falloff * spotLights[i].L * f;
	}
	
	// sphere light
	for(int i=0; i < numSphereLight; i++) {
		vec3 fragToLight = sphereLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = dot(wi, norm);
		float ratio = dist / sphereLights[i].radius;
		float ratio2 = ratio * ratio;
		
		float attenuation;
		if(ratio * cosTheta > 1)
			attenuation = cosTheta / ratio2;
		else{
			float sinTheta = sqrt(1 - cosTheta*cosTheta);
			float cotTheta = cosTheta / sinTheta;
			float x = sqrt(ratio2 - 1);
			float y = -x * cotTheta;
			
			attenuation = (1 / ( PI * ratio2)) *
				(cosTheta * acos(y) - x * sinTheta * sqrt(1 - y * y)) +
				(1 / PI) * atan(sinTheta * sqrt(1 - y * y)/x);
		}
		attenuation *= PI;
		attenuation = max(0.0, attenuation);

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += attenuation * sphereLights[i].L * f;
	}
	
	// disk light
	for(int i=0; i<numDiskLight; i++) {
		vec3 fragToLight = diskLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = dot(wi, norm);
		float sinTheta = sqrt(1 - cosTheta*cosTheta);
		float cotTheta = cosTheta / sinTheta;
		float ratio = dist / diskLights[i].radius;
		float ratio2 = ratio * ratio;
		
		float attenuation;
		if(cotTheta > 1 / ratio)
			attenuation = cosTheta / (1 + ratio2);
		else{
			float x = sqrt(1 - ratio2 * cotTheta * cotTheta);
			
			attenuation = -ratio * x * sinTheta / (PI * (1 + ratio2)) +
				(1 / PI) * atan(x * sinTheta / ratio) +
				cosTheta * (PI - acos(ratio * cotTheta)) / (PI * (1 + ratio2));
		}
		attenuation *= PI * dot(diskLights[i].dir, -wi);
		attenuation = max(0.0, attenuation);

		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += attenuation * diskLights[i].L * f;
	}
	
	// area light
	for(int i=0; i<numAreaLight; i++) {
		vec3 fragToLight = areaLights[i].position - pos;
		if(dot(-fragToLight, areaLights[i].dir) <= 0)
			continue;
		
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;
		
		// rightPyramidSolidAngle
		float a = areaLights[i].width * 0.5;
		float b = areaLights[i].height * 0.5;
		float solidAngle = 4 * asin(a*b / sqrt((a*a + dist2) * (b*b + dist2)));
		
		vec3 verticle = cross(areaLights[i].dir, areaLights[i].horizontal);
		vec3 halfWidthVec = areaLights[i].horizontal * a;
		vec3 halfHeightVec = verticle * b;
		
		vec3 p0 = areaLights[i].position - halfWidthVec - halfHeightVec;
		vec3 p1 = areaLights[i].position - halfWidthVec + halfHeightVec;
		vec3 p2 = areaLights[i].position + halfWidthVec - halfHeightVec;
		vec3 p3 = areaLights[i].position + halfWidthVec + halfHeightVec;
		
		float illuminanceFactor = solidAngle * 0.2 * (
			max(0, dot(normalize(p0 - pos), norm)) +
			max(0, dot(normalize(p1 - pos), norm)) +
			max(0, dot(normalize(p2 - pos), norm)) +
			max(0, dot(normalize(p3 - pos), norm)) +
			max(0, dot(wi, norm)) );
		
		vec3 f = BRDF(ID, norm, wo, wi, albedo, metallic, roughness, ao);
		
		result += illuminanceFactor * areaLights[i].L * f;
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

float Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = max(0, 1 - ratio4);
	return falloff * falloff;
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

float SpotLightDirFalloff(vec3 wi, int id) {
	float cosTheta = -dot(wi, spotLights[id].dir);
	if (cosTheta < spotLights[id].cosHalfAngle) return 0;
	if (cosTheta > spotLights[id].cosFalloffAngle) return 1;

	float delta = (cosTheta - spotLights[id].cosHalfAngle) /
		(spotLights[id].cosHalfAngle - spotLights[id].cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
