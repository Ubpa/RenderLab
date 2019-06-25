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

// ----------------- Uniform

struct Gooch {
	vec3 colorFactor;
	bool haveColorTexture;
    sampler2D colorTexture;
};
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

uniform Gooch gooch;

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
	vec3 color = gooch.colorFactor;
	if(gooch.haveColorTexture)
		color *= texture(gooch.colorTexture, fs_in.TexCoords).rgb;
	
	vec3 color_cool = vec3(0,0,0.55) + 0.25 * color;
	vec3 color_warm = vec3(0.3,0.3,0) + 0.25 * color;
	vec3 hightlight = vec3(2);
	
	vec3 wo = normalize(viewPos - fs_in.FragPos);

	vec3 norm = normalize(fs_in.Normal);
	//if(bsdf.haveNormalTexture) {
	//	norm = CalcBumpedNormal(norm, normalize(fs_in.Tangent), bsdf.normalTexture, fs_in.TexCoords);
	//}
	
	// 采样光源
	vec3 result = vec3(0);
	
	// point light
    for(int i = 0; i < numPointLight; i++) {
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
		
		vec3 R = reflect(-wi, norm);
		float s = clamp(100.0 * dot(R, wo) - 97.0, 0.0, 1.0);
		vec3 f = mix(color_warm, hightlight, s);
		
		result += visibility * cosTheta / attenuation * falloff * pointLights[i].L * f;
	}
	
	// directional light
	for(int i=0; i < numDirectionalLight; i++){
		vec3 wi = -normalize(directionaLights[i].dir);

		vec3 R = reflect(-wi, norm);
		float s = clamp(100.0 * dot(R, wo) - 97.0, 0.0, 1.0);
		
		vec3 f = mix(color_warm, hightlight, s);

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

		vec3 R = reflect(-wi, norm);
		float s = clamp(100.0 * dot(R, wo) - 97.0, 0.0, 1.0);
		vec3 f = mix(color_warm, hightlight, s);
		
		result += visibility * cosTheta / attenuation * falloff * spotLights[i].L * f;
	}
	
	// ambient light
	result += 0.5 * color_cool;
	
    FragColor = vec4(result, 1);
}

float Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = max(0, 1 - ratio4);
	return falloff * falloff;
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
