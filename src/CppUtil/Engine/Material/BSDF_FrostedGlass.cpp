#include <CppUtil/Engine/BSDF_FrostedGlass.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

float BSDF_FrostedGlass::GGX_D(const vec3 & h, float alpha) {
	float HoN = h.z;
	if (HoN <= 0)
		return 0;

	float cos2 = HoN * HoN;
	float alpha2 = alpha * alpha;

	float root = alpha / (cos2 * (alpha2 - 1) + 1);

	return Math::INV_PI * root * root;
}

float BSDF_FrostedGlass::GGX_G1(const vec3 & v, const vec3 & h, float alpha) {
	float NoV = v.z;
	float HoV = dot(v, h);
	if (NoV * HoV <= 0)
		return 0;

	float alpha2 = alpha * alpha;
	float tan2 = 1.f / (NoV * NoV) - 1.f;
	return 2.f / (1.f + sqrt(1 + alpha2 * tan2));
}

float BSDF_FrostedGlass::GGX_G(const vec3 & wo, const vec3 & wi, const vec3 & h, float alpha) {
	// Smith
	return GGX_G1(wo, h, alpha) * GGX_G1(wi, h, alpha);
}

float BSDF_FrostedGlass::Fr(const vec3 & v, const vec3 & h, float ior) {
	// 需要是低折射率介质中的角
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

vec3 BSDF_FrostedGlass::F(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	bool entering = wo.z > 0;
	bool isReflect = wo.z * wi.z > 0;

	vec3 color = GetColor(texcoord);
	color *= GetAO(texcoord);
	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	if (isReflect) {
		vec3 h = normalize(wo + wi);

		h *= sign(h.z);// 使 h 指向外侧
		return color * Fr(wo, h, ior) * GGX_D(h, alpha) * GGX_G(wo, wi, h, alpha) / abs(4.f * wo.z * wi.z);
	}
	else {
		float etai = 1.f, etat = ior;
		if (!entering)
			swap(etai, etat);

		vec3 h = -normalize(etai * wo + etat * wi);

		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (wo.z * wi.z));

		h *= sign(h.z);// 使 h 指向外侧
		return color * factor * ((1 - Fr(wo, h, ior)) * GGX_D(h, alpha) * GGX_G(wo, wi, h, alpha) * etat * etat) /
			(sqrtDenom * sqrtDenom);
	}
}

// probability density function
float BSDF_FrostedGlass::PDF(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	bool isReflect = wo.z * wi.z > 0;

	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	vec3 h;
	float dwh_dwi;
	if (isReflect) {
		h = normalize(wo + wi);

		dwh_dwi = 1.0f / (4.0f * abs(dot(wi, h)));
	}
	else {
		float etai = 1.f, etat = ior;
		bool entering = wo.z > 0;
		if (!entering)
			swap(etai, etat);

		h = -normalize(etai * wo + etat * wi);

		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
	}

	h *= sign(h.z);
	float Dh = GGX_D(h, alpha);

	float fr = Fr(wo, h, ior);

	return Dh * h.z * dwh_dwi * (isReflect ? fr : 1 - fr);
}

vec3 BSDF_FrostedGlass::Sample_f(const vec3 & wo, const vec2 & texcoord, vec3 & wi, float & PD) {
	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	// Sample h
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta2 = (1 - Xi1) / (Xi1*(alpha*alpha - 1) + 1);
	float cosTheta = sqrt(cosTheta2);
	float sinTheta = sqrt(1 - cosTheta2);
	float phi = 2 * Math::PI * Xi2;

	vec3 h(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	float fr = Fr(wo, h, ior);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = reflect(-wo, h);
		if (wi.z * wo.z <= 0) {
			PD = 0;
			return vec3(0);
		}

		float Dh = GGX_D(h, alpha);
		PD = Dh * h.z / (4.f * abs(dot(wo, h))) * fr;
		float Gwowih = GGX_G(wo, wi, h, alpha);
		vec3 color = GetColor(texcoord);
		color *= GetAO(texcoord);
		return color * fr * Dh * Gwowih / abs(4.f*wo.z*wi.z);
	}
	else {
		bool entering = wo.z > 0.f;
		float etai = 1.f, etat = ior;
		if (entering) {
			wi = refract(-wo, h, etai / etat);
		}
		else {
			swap(etai, etat);
			wi = refract(-wo, -h, etai / etat);
		}

		if (wi.z * wo.z >= 0 || wi == vec3(0)) {
			PD = 0;
			return vec3(0);
		}

		float Dh = GGX_D(h, alpha);
		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		float dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
		PD = Dh * h.z * dwh_dwi * (1 - fr);

		float Gwowih = GGX_G(wo, wi, h, alpha);
		float factor = abs(HoWo * HoWi / (wo.z * wi.z));
		vec3 color = GetColor(texcoord);
		color *= GetAO(texcoord);
		return color * factor * ((1 - fr) * Dh * Gwowih * etat * etat) /
			(sqrtDenom * sqrtDenom);
	}
}

vec3 BSDF_FrostedGlass::GetColor(const vec2 & texcoord) const {
	if (!colorTexture || !colorTexture->IsValid())
		return colorFactor;

	bool blend = colorTexture->GetChannel() == 4;
	return vec3(colorTexture->Sample(texcoord, blend)) * colorFactor;
}

float BSDF_FrostedGlass::GetRoughness(const vec2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessTexture->Sample(texcoord).x * roughnessFactor;
}

float BSDF_FrostedGlass::GetAO(const vec2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord).x;
}

void BSDF_FrostedGlass::ChangeNormal(const vec2 & texcoord, const vec3 & tangent, vec3 & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	vec3 tangentSpaceNormal = 2.0f * normalTexture->Sample(texcoord) - 1.0f;

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
