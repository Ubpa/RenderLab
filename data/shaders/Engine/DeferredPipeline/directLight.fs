#version 330 core

#include "../BRDF/MetalWorkflow.h"
#include "../BRDF/Frostbite.h"
#include "../BRDF/Diffuse.h"

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

// 48
struct CapsuleLight {
	vec3 p0;        // 12    0
	float radius;   //  4   12
	vec3 p1;        // 12   16
    vec3 L;         // 12   32
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

// 1040
layout (std140) uniform SpotLights {
	int numSpotLight;// 16
	SpotLight spotLights[MAX_SPOT_LIGHTS];// 128 * MAX_SPOT_LIGHTS = 128 * 8 = 1024
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

void BRDF(out vec3 diffuse, out vec3 spec, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness);

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

float SphereIlluminanceFactor(vec3 vertex, vec3 norm, vec3 center, float radius);

float AreaIlluminanceFactor(vec3 vertex, vec3 norm, vec3 lightPos, float width, float height, vec3 forward, vec3 horizontal);

// Return the closest point on the line ( without limit )
vec3 closestPointOnLine ( vec3 a, vec3 b, vec3 c)
{
    vec3 ab = b - a;
    float t = dot(c - a, ab) / dot (ab , ab);
    return a + t * ab;
}

// Return the closest point on the segment ( with limit )
vec3 closestPointOnSegment ( vec3 a, vec3 b, vec3 c)
{
    vec3 ab = b - a;
    float t = dot(c - a, ab) / dot(ab , ab);
    return a + clamp(t, 0, 1) * ab;
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
		vec3 fragToLight = pointLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		
		float falloff = Fwin(dist, pointLights[i].radius);
		
		float visibility = PointLightVisibility(-fragToLight, i);
		
		float attenuation = 1.0 / max(0.0001, dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = max(dot(wi, norm), 0);

		vec3 f, fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		f = fd + fs;
		
		result += visibility * cosTheta * attenuation * falloff * pointLights[i].L * f;
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
		vec3 fragToLight = spotLights[i].position - pos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;
		
		float distFalloff = Fwin(dist, spotLights[i].radius);
		float dirFalloff = SpotLightDirFalloff(wi, i);
		float falloff = dirFalloff * distFalloff;
		
		float attenuation = 1.0 / max(0.0001, dist2);

		float cosTheta = max(dot(wi, norm), 0);
		
		vec4 pos4 = spotLights[i].ProjView * vec4(pos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = SpotLightVisibility(normPos, cosTheta, i);

		vec3 f, fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		f = fd + fs;
		
		result += visibility * cosTheta * attenuation * falloff * spotLights[i].L * f;
	}
	
	// sphere light
	for(int i=0; i < numSphereLight; i++) {
		vec3 fragToLight = sphereLights[i].position - pos;
		
		vec3 LtoR = dot(fragToLight, R) * R -  fragToLight;
		fragToLight += saturate(sphereLights[i].radius / length(LtoR)) * LtoR;
		
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		
		float illuminanceFactor = SphereIlluminanceFactor(pos, norm, sphereLights[i].position, sphereLights[i].radius);
		
		vec3 wi = fragToLight / dist;
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		float attenuation = 1.0 / max(0.0001, dist2);
		float area = 4 * PI * sphereLights[i].radius * sphereLights[i].radius;
		result += (illuminanceFactor * fd + attenuation * fs / (area * PI)) * sphereLights[i].L;
	}
	
	// disk light
	for(int i=0; i<numDiskLight; i++) {
		vec3 fragToLight = diskLights[i].position - pos;
		
		float r2 = diskLights[i].radius * diskLights[i].radius;
		// closest point
		// d0 : light dir
		// d1 : wi
		// d2 : R
		// d3 : a*d1 + b*d2
		// dot(d3, d0) = 0 --> b = - dot(d1, d0) * a / dot(d2, d0) = k * a
		//                 --> d3 = a * (d1 + k*d2)
		// dot(d3, d3) = r2 --> a2 = r2 / (d1 + k*d2)^2
		// P : pos + t * d2
		// d4 : light to P
		// dot(d4, d0) = 0 --> dot(pos + t * d2 - L, d0) = 0
		//                 --> t * dot(d2, d0) + dot(pos - L, d0) = 0
		vec3 d0 = diskLights[i].dir;
		vec3 d1 = -normalize(fragToLight);
		vec3 d2 = R;
		float t = dot(fragToLight, d0) / dot(d2, d0);
		vec3 offset = pos + t * d2 - diskLights[i].position;
		float curR2 = saturate(dot(offset, offset) / r2) * r2;
		float k = - dot(d1, d0) / dot(d2, d0);
		vec3 d1kd2 = d1 + k * d2;
		float a2 = curR2 / dot(d1kd2, d1kd2);
		float a = sqrt(a2);
		float b = k * a;
		vec3 d3 = a * d1kd2;
		fragToLight += d3;
		
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);

		vec3 wi = fragToLight / dist;
		float cosTheta = dot(wi, norm);
		float sinTheta = sqrt(1 - cosTheta*cosTheta);
		float cotTheta = cosTheta / sinTheta;
		float ratio = dist / diskLights[i].radius;
		float ratio2 = ratio * ratio;
		
		float illuminanceFactor;
		if(cotTheta > 1 / ratio)
			illuminanceFactor = cosTheta / (1 + ratio2);
		else{
			float x = sqrt(1 - ratio2 * cotTheta * cotTheta);
			
			illuminanceFactor = -ratio * x * sinTheta / (PI * (1 + ratio2)) +
				(1 / PI) * atan(x * sinTheta / ratio) +
				cosTheta * (PI - acos(ratio * cotTheta)) / (PI * (1 + ratio2));
		}
		illuminanceFactor *= PI * dot(diskLights[i].dir, -wi);
		illuminanceFactor = max(0.0, illuminanceFactor);

		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		float attenuation = 1.0 / max(0.00001, dist2);
		float area = PI * r2;
		result += step(0, dot(-wi, diskLights[i].dir)) * (illuminanceFactor * fd + attenuation * fs / (area * PI)) * diskLights[i].L;
	}
	
	// area light
	for(int i=0; i<numAreaLight; i++) {
		float illuminanceFactor = AreaIlluminanceFactor(pos, norm,
			areaLights[i].position, areaLights[i].width, areaLights[i].height, areaLights[i].dir, areaLights[i].horizontal);
		
		vec3 wi = normalize(areaLights[i].position - pos);
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		result += illuminanceFactor * areaLights[i].L * fd;
	}
	
	// capsule light
	for(int i=0; i<numCapsuleLight; i++) {
		// area light part
		vec3 forward = normalize(closestPointOnLine(capsuleLights[i].p0, capsuleLights[i].p1, pos) - pos);
		vec3 areaCenter = (capsuleLights[i].p0 + capsuleLights[i].p1) / 2;
		vec3 left = capsuleLights[i].p0 - capsuleLights[i].p1;
		float height = length(left);
		vec3 nLeft = left / height;
		
		float areaPart = AreaIlluminanceFactor(pos, norm,
			areaCenter, 2 * capsuleLights[i].radius, height, forward, nLeft);
		
		vec3 sphereCenter = closestPointOnSegment(capsuleLights[i].p0, capsuleLights[i].p1 , pos);
		float spherePart = SphereIlluminanceFactor(pos, norm, sphereCenter, capsuleLights[i].radius);
		
		float illuminanceFactor = areaPart + spherePart;

		vec3 wi = normalize(areaCenter - pos);
		
		vec3 fd, fs;
		BRDF(fd, fs, ID, norm, wo, wi, albedo, metallic, roughness);
		
		result += illuminanceFactor * capsuleLights[i].L * fd;
	}
	
    FragColor = result;
}

// ----------------- 函数定义

void BRDF(out vec3 fd, out vec3 fs, int ID, vec3 norm, vec3 wo, vec3 wi, vec3 albedo, float metallic, float roughness) {
	if(ID == 0) {
		BRDF_MetalWorkflow(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	}
	else if(ID == 1) {
		BRDF_Diffuse(fd, fs, albedo);
	}
	else if(ID == 2) {
		BRDF_Frostbite(fd, fs, norm, wo, wi, albedo, metallic, roughness);
	}
	else{
		fd = vec3(0);
		fs = vec3(0);
	}
}

float Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = max(0, 1 - ratio4);
	return falloff * falloff;
}

float SphereIlluminanceFactor(vec3 vertex, vec3 norm, vec3 center, float radius) {
	vec3 diff = center - vertex;
	float dist = length(diff);
	vec3 wi = diff / dist;
	float cosTheta = dot(wi, norm);
	
	float ratio = dist / radius;
	float ratio2 = ratio * ratio;
	
	float illuminanceFactor;
	if(ratio * cosTheta > 1)
		illuminanceFactor = cosTheta / ratio2;
	else{
		float sinTheta = sqrt(1 - cosTheta*cosTheta);
		float cotTheta = cosTheta / sinTheta;
		float x = sqrt(ratio2 - 1);
		float y = -x * cotTheta;
			
		illuminanceFactor = (1 / ( PI * ratio2)) *
			(cosTheta * acos(y) - x * sinTheta * sqrt(1 - y * y)) +
			(1 / PI) * atan(sinTheta * sqrt(1 - y * y)/x);
	}
	illuminanceFactor *= PI;
	illuminanceFactor = max(0.0, illuminanceFactor);
	return illuminanceFactor;
}

float AreaIlluminanceFactor(vec3 vertex, vec3 norm, vec3 lightPos, float width, float height, vec3 forward, vec3 horizontal) {
	vec3 fragToLight = lightPos - vertex;
	if(dot(-fragToLight, forward) <= 0)
		return 0;
		
	float dist2 = dot(fragToLight, fragToLight);
	float dist = sqrt(dist2);
	vec3 wi = fragToLight/dist;
	
	// rightPyramidSolidAngle
	float a = width * 0.5;
	float b = height * 0.5;
	float solidAngle = 4 * asin(a*b / sqrt((a*a + dist2) * (b*b + dist2)));
	
	vec3 verticle = cross(forward, horizontal);
	vec3 halfWidthVec = horizontal * a;
	vec3 halfHeightVec = verticle * b;
	
	vec3 p0 = lightPos - halfWidthVec - halfHeightVec;
	vec3 p1 = lightPos - halfWidthVec + halfHeightVec;
	vec3 p2 = lightPos + halfWidthVec - halfHeightVec;
	vec3 p3 = lightPos + halfWidthVec + halfHeightVec;
	
	float illuminanceFactor = solidAngle * 0.2 * (
		max(0, dot(normalize(p0 - vertex), norm)) +
		max(0, dot(normalize(p1 - vertex), norm)) +
		max(0, dot(normalize(p2 - vertex), norm)) +
		max(0, dot(normalize(p3 - vertex), norm)) +
		max(0, dot(wi, norm)) );
	
	return illuminanceFactor;
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
