#include <CppUtil/Engine/BSDF_FrostedGlass.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float BSDF_FrostedGlass::Fr(const Normalf & v, const Normalf & h, float ior) {
	// 需要是低折射率介质中的角
	float cosTheta = v.Dot(h);
	if (!SurfCoord::IsEntering(v,h)) {
		Normalf vInLowIOR = Normalf::Refract(-v, -h, ior);
		if (vInLowIOR.IsZero())
			return 1.f;

		cosTheta = vInLowIOR.Dot(h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	return R0 + (1 - R0) * pow((1 - cosTheta), 5);
}

const RGBf BSDF_FrostedGlass::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	if (SurfCoord::CosTheta(wo) == 0 || SurfCoord::CosTheta(wi) == 0)
		return RGBf(0.f);

	auto color = GetColor(texcoord);
	color *= GetAO(texcoord);

	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	bool isReflect = SurfCoord::IsSameSide(wo, wi);
	if (isReflect) {
		Normalf h = (wo + wi).Normalize();
		SurfCoord::SetOurward(h);

		float bsdfVal = Fr(wo, h, ior) * ggx.D(h) * ggx.G(wo,wi,h) / abs(4.f * SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!SurfCoord::IsEntering(wo))
			swap(etai, etat);

		Normalf h = -(etai * wo + etat * wi).Normalize();
		SurfCoord::SetOurward(h);

		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));

		float bsdfVal = factor * ((1 - Fr(wo, h, ior)) * ggx.D(h) * ggx.G(wo, wi, h) * etat * etat) / (sqrtDenom * sqrtDenom);

		return color * bsdfVal;
	}
}

// probability density function
float BSDF_FrostedGlass::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	Normalf h;
	float dwh_dwi;
	bool isReflect = SurfCoord::IsSameSide(wo, wi);
	if (isReflect) {
		h = (wo + wi).Normalize();

		dwh_dwi = 1.0f / (4.0f * abs(wi.Dot(h)));
	}
	else {
		float etai = 1.f, etat = ior;
		if (!SurfCoord::IsEntering(wo))
			swap(etai, etat);

		h = -(etai * wo + etat * wi).Normalize();

		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
	}

	SurfCoord::SetOurward(h);
	float Dh = ggx.D(h);

	float fr = Fr(wo, h, ior);

	return Dh * SurfCoord::CosTheta(h) * dwh_dwi * (isReflect ? fr : 1 - fr);
}

const RGBf BSDF_FrostedGlass::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	auto h = ggx.Sample_wh();

	float fr = Fr(wo, h, ior);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = Normalf::Reflect(-wo, h);
		if (!SurfCoord::IsSameSide(wo,wi)) {
			PD = 0;
			return RGBf(0.f);
		}

		float Dh = ggx.D(h);
		PD = Dh * SurfCoord::CosTheta(h) / (4.f * abs(wo.Dot(h))) * fr;

		auto color = GetColor(texcoord);
		color *= GetAO(texcoord);
		float bsdfVal = fr * Dh * ggx.G(wo, wi, h) / abs(4.f * SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (SurfCoord::IsEntering(wo)) {
			wi = Normalf::Refract(-wo, h, etai / etat);
		}
		else {
			swap(etai, etat);
			wi = Normalf::Refract(-wo, -h, etai / etat);
		}

		if (SurfCoord::IsSameSide(wi,wo) || wi.IsZero()) {
			PD = 0;
			return RGBf(0.f);
		}

		float Dh = ggx.D(h);
		float HoWo = h.Dot(wo);
		float HoWi = h.Dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		float dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
		PD = Dh * SurfCoord::CosTheta(h) * dwh_dwi * (1 - fr);

		float Gwowih = ggx.G(wo, wi, h);
		float factor = abs(HoWo * HoWi / (SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));
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
	Normalf tangentSpaceNormal = 2.f * Vec3(rgb.r, rgb.g, rgb.b) - Vec3(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
