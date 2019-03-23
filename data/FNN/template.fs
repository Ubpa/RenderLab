#version 330 core

// ----------------- in out

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
} fs_in;

// ----------------- const

#define MAX_POINT_LIGHTS 8
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

// ----------------- struct

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

// 48
struct PointLight {
    vec3 position;	// 12	0
    vec3 L;			// 12	16
    float linear;	// 4	28
    float quadratic;// 4	32
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
	int numLight;// 16
	PointLight pointLights[MAX_POINT_LIGHTS];// 48 * MAX_POINT_LIGHTS = 48 * 8
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

uniform float lightFar;

uniform int mode;

// ----------------- declaration

vec3 CalcBumpedNormal(vec3 normal, vec3 tangent, sampler2D normalTexture, vec2 texcoord);

float GGX_D(vec3 norm, vec3 h, float alpha);
float Fr(vec3 v, vec3 h, float ior);
float GGX_G1(vec3 norm, vec3 v, vec3 h, float alpha);
float GGX_G(vec3 norm, vec3 wo, vec3 wi, vec3 h, float alpha);
vec3 BSDF(vec3 norm, vec3 wo, vec3 wi, vec3 color, float roughness, float ao, float ior);

float Visibility(vec3 lightToFrag, int id);
float Visibility(vec3 lightToFrag, samplerCube depthMap);

// template declaration
void $funcname$
(
	in float directIllum_R,
	in float directIllum_G,
	in float directIllum_B,
	in float pos_x,
	in float pos_y,
	in float pos_z,
	in float viewDir_x,
	in float viewDir_y,
	in float viewDir_z,
	in float normal_x,
	in float normal_y,
	in float normal_z,
	in float matColor_R,
	in float matColor_G,
	in float matColor_B,
	in float IOR,
	in float roughness,
	
	out float indirectIllum_R,
	out float indirectIllum_G,
	out float indirectIllum_B
);

// ----------------- main

void main() {
	// get attr
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
	
	// sample point light
	vec3 result = vec3(0);
    for(int i = 0; i < numLight; i++) {
		vec3 fragToLight = pointLights[i].position - fs_in.FragPos;
		float dist2 = dot(fragToLight, fragToLight);
		float dist = sqrt(dist2);
		vec3 wi = fragToLight / dist;
		
		float visibility = Visibility(-fragToLight, i);
		if(visibility==0)
			continue;

		vec3 f = BSDF(norm, wo, wi, color, roughness, ao, bsdf.ior);

		float cosTheta = max(dot(wi, norm), 0);
		
		float attenuation = 1.0f + pointLights[i].linear * dist + pointLights[i].quadratic * dist2;
		
		result += visibility * cosTheta / attenuation * f * pointLights[i].L;
	}
	
	vec3 indirectIllum;
	
	$funcname$
	(
		// input
		result.r,
		result.g,
		result.b,
		fs_in.FragPos.x,
		fs_in.FragPos.y,
		fs_in.FragPos.z,
		wo.x,
		wo.y,
		wo.z,
		norm.x,
		norm.y,
		norm.z,
		color.r,
		color.g,
		color.b,
		bsdf.ior,
		roughness,
		
		// output
		indirectIllum.r,
		indirectIllum.g,
		indirectIllum.b
	);
	
	indirectIllum = clamp(indirectIllum, 0, 1);
	
	// gamma correction
	vec3 finalColor;
	if(mode==0)
		finalColor = result;
	else if(mode==1)
		finalColor = indirectIllum;
	else
		finalColor = result + indirectIllum;
		
    FragColor = vec4(sqrt(finalColor), 1.0);
}

// ----------------- definition

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

float Visibility(vec3 lightToFrag, int id){
	if(id == 0) {
		return Visibility(lightToFrag, pointLightDepthMap0);
	} else if(id == 1) {
		return Visibility(lightToFrag, pointLightDepthMap1);
	} else if(id == 2) {
		return Visibility(lightToFrag, pointLightDepthMap2);
	} else if(id == 3) {
		return Visibility(lightToFrag, pointLightDepthMap3);
	} else if(id == 4) {
		return Visibility(lightToFrag, pointLightDepthMap4);
	} else if(id == 5) {
		return Visibility(lightToFrag, pointLightDepthMap5);
	} else if(id == 6) {
		return Visibility(lightToFrag, pointLightDepthMap6);
	} else if(id == 7) {
		return Visibility(lightToFrag, pointLightDepthMap7);
	}else 
		return 1;// not support id
}

float Visibility(vec3 lightToFrag, samplerCube depthMap) {
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

