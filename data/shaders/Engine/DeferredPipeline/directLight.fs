#version 330 core

#include "../BRDF/MetalWorkflow.h"
#include "../BRDF/Frostbite.h"
#include "../BRDF/Diffuse.h"
#include "../../Math/intersect.h"
#include "../../Math/Segment.h"

#include "../Light/AreaLight.h"
#include "../Light/SphereLight.h"
#include "../Light/CapsuleLight.h"
#include "../Light/PointLight.h"
#include "../Light/SpotLight.h"
#include "../Light/DiskLight.h"
#include "../Light/LTC.h"

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
#define MAX_CAPSULE_LIGHTS 8

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

// 96
struct DirectionalLight{
	vec3 L;         // 12   0
	vec3 dir;       // 12   16
	mat4 ProjView;  // 64   32
};

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

// 272
layout (std140) uniform PointLights { 
	int numPointLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 32 * MAX_POINT_LIGHTS = 32 * 8 = 256
};

// 784
layout (std140) uniform DirectionalLights {
	int numDirectionalLight;// 16
	DirectionalLight directionaLights[MAX_DIRECTIONAL_LIGHTS];// 96 * MAX_DIRECTIONAL_LIGHTS = 96 * 8 = 768
};

// 912
layout (std140) uniform SpotLights {
	int numSpotLight;// 16
	SpotLight spotLights[MAX_SPOT_LIGHTS];// 128 * MAX_SPOT_LIGHTS = 112 * 8 = 896
};

// 272
layout (std140) uniform SphereLights {
	int numSphereLight;// 16
	SphereLight sphereLights[MAX_SPHERE_LIGHTS];// 32 * MAX_SPHERE_LIGHTS = 32 * 8 = 256
};

// 400
layout (std140) uniform DiskLights {
	int numDiskLight;// 16
	DiskLight diskLights[MAX_DISK_LIGHTS];// 48 * MAX_DISK_LIGHTS = 48 * 8 = 384
};

// 528
layout (std140) uniform AreaLights {
	int numAreaLight;// 16
	AreaLight areaLights[MAX_AREA_LIGHTS];// 64 * MAX_AREA_LIGHTS = 64 * 8 = 512
};

// 400
layout (std140) uniform CapsuleLights {
	int numCapsuleLight;// 16
	CapsuleLight capsuleLights[MAX_CAPSULE_LIGHTS];// 48 * MAX_CAPSULE_LIGHTS = 48 * 8 = 256
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

vec3 BRDFd(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
vec3 BRDFs(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);
void BRDF(out vec3 diffuse, out vec3 spec, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

float Fwin(float d, float radius);
float PointLightVisibility(vec3 pos, vec3 lightToFrag, int id);
float PointLightVisibility(vec3 pos, vec3 lightToFrag, samplerCube depthMap);

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

float DiskIlluminanceFactor(vec3 wi, vec3 norm, float dist, float radius, vec3 forward);

// ----------------- 主函数

void main() {
	// 获取属性值
	vec4 data0 = texture(GBuffer0, TexCoords);
	vec4 data1 = texture(GBuffer1, TexCoords);
	vec4 data2 = texture(GBuffer2, TexCoords);
	vec4 data3 = texture(GBuffer3, TexCoords);
	
	int ID = int(data0.w);
	vec3 pos = data0.xyz;
	vec3 norm = normalize(data1.xyz);
	vec3 albedo = data2.xyz;
	float roughness = data1.w;
	roughness = mix(0.02, roughness, roughness);
	float metallic = data2.w;
	float ao = data3.w;
	
	vec3 wo = normalize(viewPos - pos);
	vec3 R = reflect(-wo, norm);
	
	if(ID == 3) { // emission
		FragColor = dot(norm, wo) > 0 ? albedo : vec3(0);
		return;
	}
	
	// for ID 0, 1, 2
	
	// 采样光源
	vec3 result = vec3(0);
	
	// point light
    for(int i = 0; i < numPointLight; i++) {
		vec3 PtoL = pointLights[i].position - pos;

		float visibility = PointLightVisibility(pos, -PtoL, i);

		vec3 wi = normalize(PtoL);
		float cosTheta = max(0, dot(wi, norm));

		vec3 f, fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		f = fd + fs;

		vec3 illuminance = PointLight_Illuminance(pointLights[i], pos);
		
		result += visibility * cosTheta * illuminance * f;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++) {
		vec3 wi = directionaLights[i].dir;
		
		vec3 f, fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		f = fd + fs;

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = directionaLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = DirectionalLightVisibility(normPos, cosTheta, i);
		
		result += visibility * cosTheta * f * directionaLights[i].L;
	}
	
	// spot light
	for(int i=0; i < numSpotLight; i++) {
		vec3 illuminance = SpotLight_Illuminance(spotLights[i], pos);

		vec3 PtoL = spotLights[i].position - pos;
		vec3 wi = normalize(PtoL);

		float cosTheta = max(0, dot(wi, norm));
		
		vec4 pos4 = spotLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);

		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		vec3 f = fd + fs;
		
		result += visibility * cosTheta * illuminance * f;
	}
	
	// sphere light
	for(int i=0; i < numSphereLight; i++) {
		vec3 illuminanceD = SphereLight_Illuminance(sphereLights[i], pos, norm);

		vec3 MRP = SphereLight_MRP(sphereLights[i], pos, R);
		vec3 PtoL = MRP - pos;
		float dist2 = dot(PtoL, PtoL);
		float dist = sqrt(dist2);
		vec3 wi = PtoL / dist;
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		vec3 illuminanceS = sphereLights[i].luminance / max(0.0001, dist2);
		float cosTheta = max(0, dot(wi, norm));
		float normalizeFactor = SphereLight_FsNormFactor(sphereLights[i], pos, roughness);
		result += illuminanceD * fd + normalizeFactor * cosTheta * illuminanceS * fs;
	}
	
	// disk light
	for(int i=0; i<numDiskLight; i++) {
		vec3 illuminanceD = DiskLight_Illuminance(diskLights[i], pos, norm);

		vec3 MRP = DiskLight_MRP(diskLights[i], pos, R);
		vec3 PtoL = MRP - pos;
		float dist2 = dot(PtoL, PtoL);
		float dist = sqrt(dist2);
		vec3 wi = PtoL / dist;

		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);

		float attenuation = step(0, dot(-wi, diskLights[i].dir)) / max(0.00001, dist2);
		vec3 illuminanceS = diskLights[i].luminance * attenuation;
		float cosTheta = max(0, dot(wi, norm));
		result += illuminanceD * fd + cosTheta * illuminanceS * fs;
	}
	
	// area light
	for(int i=0; i<numAreaLight; i++) {
#if 1
		vec3 F0 = MetalWorkflowF0(albedo, metallic);
		vec3 spec = LTC_Spec(norm, wo, pos, F0, roughness, areaLights[i]);
		vec3 FrR = SchlickFrR(wo, norm, F0, roughness);
		vec3 diffuse = (1 - metallic) * albedo * FrR * INV_PI * LTC_Diffuse(norm, wo, pos, roughness, areaLights[i]);
		result += diffuse + spec;
#else
		vec3 illuminanceD = AreaLight_Illuminance(areaLights[i], pos, norm);
		
		vec3 MRP = AreaLight_MRP(areaLights[i], pos, R);
		vec3 PtoL = MRP - pos;
		float dist2 = dot(PtoL, PtoL);
		float dist = sqrt(dist2);
		vec3 wi = PtoL / dist;
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		float attenuation = step(0, dot(-wi, areaLights[i].dir)) / max(0.0001, dist2);
		vec3 illuminanceS = areaLights[i].luminance * attenuation;
		float cosTheta = max(0, dot(wi, norm));
		result += illuminanceD * fd + cosTheta * illuminanceS * fs;
#endif
	}
	
	// capsule light
	for(int i=0; i<numCapsuleLight; i++) {
		float r2 = capsuleLights[i].radius * capsuleLights[i].radius;
		float sphereArea = FOUR_PI * r2;
		float rectArea = 2 * capsuleLights[i].radius * capsuleLights[i].height;
		float capsuleArea = CapsuleLight_Area(capsuleLights[i]);

		{// diffuse
			{// area
				vec3 forward = normalize(Line_ClosestPoint(Line_2P(capsuleLights[i].p0, capsuleLights[i].p1), pos) - pos);
				vec3 areaCenter = (capsuleLights[i].p0 + capsuleLights[i].p1) / 2;
				vec3 left = capsuleLights[i].p0 - capsuleLights[i].p1;
				float height = length(left);
				vec3 nLeft = left / height;
				AreaLight areaLight = AreaLight(areaCenter, 2 * capsuleLights[i].radius, forward, height, nLeft, capsuleLights[i].L * capsuleArea / rectArea);

				vec3 illuminance = AreaLight_Illuminance(areaLight, pos, norm);

				vec3 PtoL = areaCenter - pos;
				float dist2 = dot(PtoL, PtoL);
				float dist = sqrt(dist2);
				vec3 wi = PtoL / dist;

				vec3 fd = BRDFd(ID, norm, wo, wi, albedo, metallic, roughness);

				result += illuminance * fd;
			}
			{// sphere
				vec3 sphereCenter = Segment_ClosestPoint(Segment(capsuleLights[i].p0, capsuleLights[i].p1), pos);
				SphereLight sphereLight = SphereLight(sphereCenter, capsuleLights[i].L * capsuleArea / sphereArea, capsuleLights[i].radius);

				vec3 illuminance = SphereLight_Illuminance(sphereLight, pos, norm);

				vec3 PtoL = sphereCenter - pos;
				float dist2 = dot(PtoL, PtoL);
				float dist = sqrt(dist2);
				vec3 wi = PtoL / dist;

				vec3 fd = BRDFd(ID, norm, wo, wi, albedo, metallic, roughness);

				result += illuminance * fd;
			}
		}
		{// specular
			vec3 MRP = CapsuleLight_MRP(capsuleLights[i], pos, R);
			//result = MRP;
			vec3 PtoL = MRP - pos;
			float dist2 = dot(PtoL, PtoL);
			float dist = sqrt(dist2);
			vec3 wi = PtoL / dist;

			vec3 fs = BRDFs(ID, norm, wo, wi, albedo, metallic, roughness);
#define MODE 0
#if MODE == 0 // average distance
			float avrgDist2 = CapsuleLight_AvrgDist2(capsuleLights[i], pos);
			float attenuation = 1.0 / max(0.0001, avrgDist2);
#elif MODE == 1 // mid point
			vec3 center = 0.5 * (capsuleLights[i].p1 + capsuleLights[i].p0);
			vec3 PtoC = center - pos;
			float distToC2 = dot(PtoC, PtoC);
			float attenuation = 1.0 / max(0.0001, distToC2);
#else // MRP
			float attenuation = 1.0 / max(0.0001, dist2);
#endif
			float cosTheta = max(0, dot(wi, norm));
			result += cosTheta * attenuation * (sphereArea / capsuleArea) * fs * capsuleLights[i].L;
		}
	}
	
    FragColor = result;
}

// ----------------- 函数定义

void BRDF(out vec3 fd, out vec3 fs, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if(ID == 0)
		BRDF_MetalWorkflow(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	else if(ID == 1)
		BRDF_Diffuse(fd, fs, albedo);
	else if(ID == 2)
		BRDF_Frostbite(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	else{
		fd = vec3(0);
		fs = vec3(0);
	}
}

vec3 BRDFd(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if (ID == 0)
		return BRDFd_MetalWorkflow(wo, wi, albedo, metallic);
	else if (ID == 1)
		return BRDFd_Diffuse(albedo);
	else if (ID == 2)
		return BRDFd_Frostbite(norm, wo, wi, albedo, metallic, roughness);
	else
		return vec3(0);
}

vec3 BRDFs(int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if (ID == 0)
		return BRDFs_MetalWorkflow(norm, wo, wi, albedo, metallic, roughness);
	else if (ID == 1)
		return BRDFs_Diffuse(albedo);
	else if (ID == 2)
		return BRDFs_Frostbite(norm, wo, wi, albedo, metallic, roughness);
	else
		return vec3(0);
}

float DiskIlluminanceFactor(vec3 wi, vec3 norm, float dist, float radius, vec3 forward) {
	float cosTheta = dot(wi, norm);
	float sinTheta = sqrt(1 - cosTheta * cosTheta);
	float cotTheta = cosTheta / sinTheta;
	float ratio = dist / radius;
	float ratio2 = ratio * ratio;

	float illuminanceFactor;
	if (cotTheta > 1 / ratio)
		illuminanceFactor = cosTheta / (1 + ratio2);
	else {
		float x = sqrt(1 - ratio2 * cotTheta * cotTheta);

		illuminanceFactor = -ratio * x * sinTheta / (PI * (1 + ratio2)) +
			(1 / PI) * atan(x * sinTheta / ratio) +
			cosTheta * (PI - acos(ratio * cotTheta)) / (PI * (1 + ratio2));
	}
	illuminanceFactor *= PI * dot(forward, -wi);
	illuminanceFactor = max(0.0, illuminanceFactor);

	return illuminanceFactor;
}

// ------------------------ Visibility ------------------------ 

float PointLightVisibility(vec3 pos, vec3 lightToFrag, int id) {
	if(id == 0) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap0);
	} else if(id == 1) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap1);
	} else if(id == 2) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap2);
	} else if(id == 3) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap3);
	} else if(id == 4) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap4);
	} else if(id == 5) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap5);
	} else if(id == 6) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap6);
	} else if(id == 7) {
		return PointLightVisibility(pos, lightToFrag, pointLightDepthMap7);
	}else 
		return 1;// not support id
}

float PointLightVisibility(vec3 pos, vec3 lightToFrag, samplerCube depthMap) {
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
