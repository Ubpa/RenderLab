#version 330 core

// ----------------- 输入输出

out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
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

struct BSDF_Diffuse {
	vec3 colorFactor;
	
	bool haveAlbedoTexture;
    sampler2D albedoTexture;// 0
};

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

// 64
struct SpotLight{
    vec3 position;         // 12     0
	vec3 dir;              // 12    16
    vec3 L;                // 12    32
    float linear;	       // 4     44
    float quadratic;       // 4     48
	float cosHalfAngle;    // 4     52
	float cosFalloffAngle; // 4     56
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
	int numPointsLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 8 = 384
};

// 784
layout (std140) uniform DirectionalLights{
	int numDirectionalLight;// 16
	DirectionalLight directionaLights[MAX_DIRECTIONAL_LIGHTS];// 96 * MAX_DIRECTIONAL_LIGHTS = 96 * 8 = 768
};

// 528
layout (std140) uniform SpotLights{
	int numSpotLight;// 16
	SpotLight spotLights[MAX_SPOT_LIGHTS];// 64 * MAX_SPOT_LIGHTS = 48 * 8 = 512
};

uniform BSDF_Diffuse bsdf;

uniform samplerCube pointLightDepthMap0; // 1
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

uniform float lightFar;

// ----------------- 函数声明

float PointLightVisibility(vec3 lightToFrag, int id);
float PointLightVisibility(vec3 lightToFrag, samplerCube depthMap);

float DirectionalLightVisibility(vec3 normPos, float cosTheta, int id);
float DirectionalLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);

float SpotLightVisibility(vec3 normPos, float cosTheta, int id);
float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap);
float SpotLightFalloff(vec3 wi, int id);

// ----------------- 主函数

void main() {
	// 获取属性值
	vec3 albedo = bsdf.colorFactor;
	if(bsdf.haveAlbedoTexture) {
		albedo *= texture(bsdf.albedoTexture, fs_in.TexCoords).xyz;
	}
	
	vec3 diffuse = albedo * INV_PI;
	
	vec3 norm = normalize(fs_in.Normal);
	
	// 采样光源
	vec3 result = vec3(0);
	
	// point light
    for(int i = 0; i < numPointsLight; i++) {
		vec3 fragToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;
		
		float visibility = PointLightVisibility(-fragToLight, i);

		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += visibility * cosTheta / attenuation * diffuse * pointLights[i].L;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++){
		vec3 wi = -normalize(directionaLights[i].dir);

		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
		vec4 pos4 = directionaLights[i].ProjView * vec4(fs_in.FragPos, 1);
		vec3 normPos = ((pos4.xyz / pos4.w) + 1) / 2;
		float visibility = DirectionalLightVisibility(normPos, cosTheta, i);
		
		result += visibility * cosTheta * diffuse * directionaLights[i].L;
	}
	
	// spot light
	for(int i=0; i < numSpotLight; i++){
		vec3 fragToLight = spotLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight/dist;

		float cosTheta = max(dot(wi, normalize(fs_in.Normal)), 0);
		
		float attenuation = 1.0f + spotLights[i].linear * dist + spotLights[i].quadratic * dist2;
		
		result += SpotLightFalloff(wi, i) * cosTheta / attenuation * diffuse * spotLights[i].L;
	}
	
	// gamma 校正
	FragColor = vec4(sqrt(result), 1.0);
}

// ----------------- 函数定义

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
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap0);
	} else if(id == 1) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap1);
	} else if(id == 2) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap2);
	} else if(id == 3) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap3);
	} else if(id == 4) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap4);
	} else if(id == 5) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap5);
	} else if(id == 6) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap6);
	} else if(id == 7) {
		return SpotLightVisibility(normPos, cosTheta, directionalLightDepthMap7);
	}else
		return 1;// not support id
}

float SpotLightVisibility(vec3 normPos, float cosTheta, sampler2D depthMap){
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

float SpotLightFalloff(vec3 wi, int id){
	float cosTheta = -dot(wi, spotLights[id].dir);
	if (cosTheta < spotLights[id].cosHalfAngle) return 0;
	if (cosTheta > spotLights[id].cosFalloffAngle) return 1;

	float delta = (cosTheta - spotLights[id].cosHalfAngle) /
		(spotLights[id].cosHalfAngle - spotLights[id].cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
