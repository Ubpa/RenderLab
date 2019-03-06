#include <CppUtil/Engine/BSDF_FrostedGlass.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

// good
float BSDF_FrostedGlass::D(const vec3 & h) {
	float HoN = h.z;
	if (HoN <= 0)
		return 0;

	float cos2 = HoN * HoN;
	float tan2 = 1.f / cos2 - 1;

	float root = alpha / (cos2 * (alpha * alpha + tan2));

	return Math::INV_PI * root * root;
}

// good
float BSDF_FrostedGlass::G1(const vec3 & v, const vec3 & h) {
	float NoV = v.z;
	float HoV = dot(v, h);
	if (NoV * HoV <= 0)
		return 0;

	float alpha2 = alpha * alpha;
	float tan2 = 1.f / (NoV * NoV) - 1.f;
	return 2.f / (1.f + sqrt(1 + alpha2 * tan2));
}

// good
float BSDF_FrostedGlass::G(const vec3 & wo, const vec3 & wi, const vec3 & h) {
	// G
	return G1(wo, h) * G1(wi, h);
}

// good
float BSDF_FrostedGlass::Fr(const glm::vec3 & v, const vec3 & h) {
	float cosTheta = dot(v, h);
	bool entering = cosTheta > 0.0f;
	float ei = 1.f, et = ior;
	if (!entering)
		swap(ei, et);

	float c = abs(cosTheta);
	float etaTI = et / ei;
	float g2 = etaTI * etaTI - 1 + c * c;
	if (g2 < 0)
		return 1;

	float g = sqrt(g2);
	float gPc = g + c;
	float gMc = g - c;

	return 0.5f * pow(gMc / gPc, 2) * (1 + pow((c*gPc - 1) / (c*gMc + 1), 2));
}

vec3 BSDF_FrostedGlass::F(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	bool entering = wo.z > 0;
	bool isReflect = wo.z * wi.z > 0;

	if (isReflect)
	{
		vec3 h = normalize(wo + wi);
		h *= sign(h.z);

		return albedo * Fr(wo, h) * D(h) * G(wo, wi, h) / abs(4.f * wo.z * wi.z);
	} else {
		float etai = 1.f, etat = ior;
		if (!entering)
			swap(etai, etat);

		vec3 h = -normalize(etai * wo + etat * wi);
		h *= sign(h.z);

		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (wo.z * wi.z));
		return albedo * factor * ((1 - Fr(wo, h)) * D(h) * G(wo, wi, h) * etat * etat) /
			(sqrtDenom * sqrtDenom);
	}
}

// probability density function
float BSDF_FrostedGlass::PDF(const vec3 & wo, const vec3 & wi, const vec2 & texcoord) {
	bool isReflect = wo.z * wi.z > 0;

	vec3 h;
	float dwh_dwi;
	if (isReflect)
	{
		h = normalize(wo + wi);
		//if (!entering)
		//	wh *= -1.0f;

		dwh_dwi = 1.0f / (4.0f * abs(dot(wi, h)));
	}
	else
	{
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
	float Dh = D(h);

	float fr = Fr(wo, h);

	return Dh * h.z * dwh_dwi * (isReflect ? fr : 1 - fr);
}

// PD is probability density
// return albedo
vec3 BSDF_FrostedGlass::Sample_f(const vec3 & wo, const vec2 & texcoord, vec3 & wi, float & PD) {
	// Sample h
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta2 = (1 - Xi1) / (Xi1*(alpha*alpha - 1) + 1);
	float cosTheta = sqrt(cosTheta2);
	float sinTheta = sqrt(1 - cosTheta2);
	float phi = 2 * Math::PI * Xi2;

	vec3 h(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	float fr = Fr(wo, h);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = reflect(-wo, h);
		if (wi.z * wo.z <= 0) {
			PD = 0;
			return vec3(0);
		}

		float Dh = D(h);
		PD = Dh * h.z / (4.f * abs(dot(wo, h))) * fr;
		float Gwowih = G(wo, wi, h);
		return albedo * fr * Dh * Gwowih / abs(4.f*wo.z*wi.z);
	}
	else {
		bool entering = wo.z > 0.f;
		float etai = 1.f, etat = ior;
		if (!entering)
			swap(etai, etat);

		wi = refract(-wo, h, etat / etai);
		if (wi.z * wo.z >= 0 || wi == vec3(0)) {
			PD = 0;
			return vec3(0);
		}

		float Dh = D(h);
		float ph = Dh * h.z;
		float HoWo = dot(h, wo);
		float HoWi = dot(h, wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		float dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
		PD = ph * dwh_dwi * (1 - fr);

		float Gwowih = G(wo, wi, h);
		float factor = abs(HoWo * HoWi / (wo.z * wi.z));
		return albedo * factor * ((1 - fr) * Dh * Gwowih * etat * etat) /
			(sqrtDenom * sqrtDenom);
	}
}
