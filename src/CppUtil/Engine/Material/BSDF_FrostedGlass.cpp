#include <CppUtil/Engine/BSDF_FrostedGlass.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float BSDF_FrostedGlass::GGX_D(const Normalf & h, float alpha) {
	float HoN = h.z;
	if (HoN <= 0)
		return 0;

	float cos2 = HoN * HoN;
	float alpha2 = alpha * alpha;

	float root = alpha / (cos2 * (alpha2 - 1) + 1);

	return Math::INV_PI * root * root;
}

float BSDF_FrostedGlass::GGX_G1(const Normalf & v, const Normalf & h, float alpha) {
	float NoV = v.z;
	float HoV = v.Dot(h);
	if (NoV * HoV <= 0)
		return 0;

	float alpha2 = alpha * alpha;
	float tan2 = 1.f / (NoV * NoV) - 1.f;
	return 2.f / (1.f + sqrt(1 + alpha2 * tan2));
}

float BSDF_FrostedGlass::GGX_G(const Normalf & wo, const Normalf & wi, const Normalf & h, float alpha) {
	return GGX_G1(wo, h, alpha) * GGX_G1(wi, h, alpha);
}

float BSDF_FrostedGlass::Fr(const Normalf & v, const Normalf & h, float ior) {
	// 需要是低折射率介质中的角
	float cosTheta = v.Dot(h);
	bool entering = cosTheta > 0.0f;
	
	if (!entering) {
		Normalf vInLowIOR = Normalf::Refract(-v, -h, ior);
		if (vInLowIOR.IsZero())
			return 1.f;

		cosTheta = vInLowIOR.Dot(h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	return R0 + (1 - R0) * pow((1 - cosTheta), 5);
}

const RGBf BSDF_FrostedGlass::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	if (wo.z == 0 || wi.z == 0)
		return RGBf(0.f);

	bool entering = wo.z > 0;
	bool isReflect = wo.z * wi.z > 0;

	auto color = GetColor(texcoord);
	color *= GetAO(texcoord);
	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	if (isReflect) {
		Normalf h = (wo + wi).Norm();

		h *= Math::sgn(h.z);// 使 h 指向外侧
		float bsdfVal = Fr(wo, h, ior) * GGX_D(h, alpha) * GGX_G(wo, wi, h, alpha) / abs(4.f * wo.z * wi.z);
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!entering)
			swap(etai, etat);

		Normalf h = -(etai * wo + etat * wi).Norm();
		h *= Math::sgn(h.z);// 使 h 指向外侧

		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (wo.z * wi.z));

		float bsdfVal = factor * ((1 - Fr(wo, h, ior)) * GGX_D(h, alpha) * GGX_G(wo, wi, h, alpha) * etat * etat) / (sqrtDenom * sqrtDenom);

		return color * bsdfVal;
	}
}

// probability density function
float BSDF_FrostedGlass::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	bool isReflect = wo.z * wi.z > 0;

	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	Normalf h;
	float dwh_dwi;
	if (isReflect) {
		h = (wo + wi).Norm();

		dwh_dwi = 1.0f / (4.0f * abs(wi.Dot(h)));
	}
	else {
		float etai = 1.f, etat = ior;
		bool entering = wo.z > 0;
		if (!entering)
			swap(etai, etat);

		h = -(etai * wo + etat * wi).Norm();

		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
	}

	h *= Math::sgn(h.z);
	float Dh = GGX_D(h, alpha);

	float fr = Fr(wo, h, ior);

	return Dh * h.z * dwh_dwi * (isReflect ? fr : 1 - fr);
}

const RGBf BSDF_FrostedGlass::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) {
	float roughness = GetRoughness(texcoord);
	float alpha = roughness * roughness;

	// Sample h
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float cosTheta2 = (1 - Xi1) / (Xi1*(alpha*alpha - 1) + 1);
	float cosTheta = sqrt(cosTheta2);
	float sinTheta = sqrt(1 - cosTheta2);
	float phi = 2 * Math::PI * Xi2;

	Normalf h(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	float fr = Fr(wo, h, ior);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = Normalf::Reflect(-wo, h);
		if (wi.z * wo.z <= 0) {
			PD = 0;
			return RGBf(0.f);
		}

		float Dh = GGX_D(h, alpha);
		PD = Dh * h.z / (4.f * abs(wo.Dot(h))) * fr;
		float Gwowih = GGX_G(wo, wi, h, alpha);
		auto color = GetColor(texcoord);
		color *= GetAO(texcoord);
		float bsdfVal = fr * Dh * Gwowih / abs(4.f*wo.z*wi.z);
		return bsdfVal * color;
	}
	else {
		bool entering = wo.z > 0.f;
		float etai = 1.f, etat = ior;
		if (entering) {
			wi = Normalf::Refract(-wo, h, etai / etat);
		}
		else {
			swap(etai, etat);
			wi = Normalf::Refract(-wo, -h, etai / etat);
		}

		if (wi.z * wo.z >= 0 || wi.IsZero()) {
			PD = 0;
			return RGBf(0.f);
		}

		float Dh = GGX_D(h, alpha);
		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		float dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
		PD = Dh * h.z * dwh_dwi * (1 - fr);

		float Gwowih = GGX_G(wo, wi, h, alpha);
		float factor = abs(HoWo * HoWi / (wo.z * wi.z));
		auto color = GetColor(texcoord);
		color *= GetAO(texcoord);
		float bsdfVal = factor * ((1 - fr) * Dh * Gwowih * etat * etat) /
			(sqrtDenom * sqrtDenom);
		return bsdfVal * color;
	}
}

const RGBf BSDF_FrostedGlass::GetColor(const Point2 & texcoord) const {
	if (!colorTexture || !colorTexture->IsValid())
		return colorFactor;

	return colorFactor * (colorTexture->Sample(texcoord, Image::Mode::BILINEAR)).ToRGB();
}

float BSDF_FrostedGlass::GetRoughness(const Point2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR).r * roughnessFactor;
}

float BSDF_FrostedGlass::GetAO(const Point2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

void BSDF_FrostedGlass::ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
	Normalf tangentSpaceNormal = 2.f * Vec3(rgb.r, rgb.g, rgb.b) - 1.f;

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
