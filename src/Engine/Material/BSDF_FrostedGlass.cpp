#include <Engine/BSDF_FrostedGlass.h>

#include <Basic/Math.h>
#include <Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float BSDF_FrostedGlass::Fr(const Ubpa::normalf & v, const Ubpa::normalf & h, float ior) {
	// 需要是低折射率介质中的角
	float cosTheta = v.dot(h);
	if (!SurfCoord::IsEntering(v,h)) {
		Ubpa::normalf vInLowIOR = Ubpa::normalf::refract(-v, -h, ior);
		if (vInLowIOR.is_all_zero())
			return 1.f;

		cosTheta = vInLowIOR.dot(h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	return R0 + (1 - R0) * pow((1 - cosTheta), 5);
}

const Ubpa::rgbf BSDF_FrostedGlass::F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	if (SurfCoord::CosTheta(wo) == 0 || SurfCoord::CosTheta(wi) == 0)
		return Ubpa::rgbf(0.f);

	auto color = GetColor(texcoord);
	color *= GetAO(texcoord);

	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	bool isReflect = SurfCoord::IsSameSide(wo, wi);
	if (isReflect) {
		Ubpa::normalf h = (wo + wi).normalize();
		SurfCoord::SetOurward(h);

		float bsdfVal = Fr(wo, h, ior) * ggx.D(h) * ggx.G_Smith(wo,wi,h) / abs(4.f * SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!SurfCoord::IsEntering(wo))
			swap(etai, etat);

		Ubpa::normalf h = -(etai * wo + etat * wi).normalize();
		if (SurfCoord::IsOnSurf(h))
			return Ubpa::rgbf(0.f);

		SurfCoord::SetOurward(h);

		float HoWo = h.dot(wo);
		float HoWi = h.dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;

		float factor = abs(HoWo * HoWi / (SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));

		float bsdfVal = factor * ((1 - Fr(wo, h, ior)) * ggx.D(h) * ggx.G_Smith(wo, wi, h) * etat * etat) / (sqrtDenom * sqrtDenom);

		return color * bsdfVal;
	}
}

// probability density function
float BSDF_FrostedGlass::PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	Ubpa::normalf h;
	float dwh_dwi;
	bool isReflect = SurfCoord::IsSameSide(wo, wi);
	if (isReflect) {
		h = (wo + wi).normalize();

		dwh_dwi = 1.0f / (4.0f * abs(wi.dot(h)));
	}
	else {
		float etai = 1.f, etat = ior;
		if (!SurfCoord::IsEntering(wo))
			swap(etai, etat);

		h = -(etai * wo + etat * wi).normalize();

		float HoWo = h.dot(wo);
		float HoWi = h.dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
	}

	SurfCoord::SetOurward(h);
	float Dh = ggx.D(h);

	float fr = Fr(wo, h, ior);

	return Dh * SurfCoord::CosTheta(h) * dwh_dwi * (isReflect ? fr : 1 - fr);
}

const Ubpa::rgbf BSDF_FrostedGlass::Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	auto h = ggx.Sample_wh();

	float fr = Fr(wo, h, ior);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = Ubpa::normalf::reflect(-wo, h);
		if (!SurfCoord::IsSameSide(wo,wi)) {
			PD = 0;
			return Ubpa::rgbf(0.f);
		}

		float Dh = ggx.D(h);
		PD = Dh * SurfCoord::CosTheta(h) / (4.f * abs(wo.dot(h))) * fr;

		auto color = GetColor(texcoord);
		color *= GetAO(texcoord);
		float bsdfVal = fr * Dh * ggx.G_Smith(wo, wi, h) / abs(4.f * SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;    
		if (SurfCoord::IsEntering(wo)) {
			wi = Ubpa::normalf::refract(-wo, h, etai / etat);
		}
		else {
			swap(etai, etat);
			wi = Ubpa::normalf::refract(-wo, -h, etai / etat);
		}

		if (SurfCoord::IsSameSide(wi,wo) || wi.is_all_zero()) {
			PD = 0;
			return Ubpa::rgbf(0.f);
		}

		float Dh = ggx.D(h);
		float HoWo = h.dot(wo);
		float HoWi = h.dot(wi);
		float sqrtDenom = etai * HoWo + etat * HoWi;
		float dwh_dwi = (etat * etat * abs(HoWi)) / (sqrtDenom * sqrtDenom);
		PD = Dh * SurfCoord::CosTheta(h) * dwh_dwi * (1 - fr);

		float Gwowih = ggx.G_Smith(wo, wi, h);
		float factor = abs(HoWo * HoWi / (SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));
		auto color = GetColor(texcoord);
		color *= GetAO(texcoord);
		float bsdfVal = factor * ((1 - fr) * Dh * Gwowih * etat * etat) /
			(sqrtDenom * sqrtDenom);
		return bsdfVal * color;
	}
}

const Ubpa::rgbf BSDF_FrostedGlass::GetColor(const Ubpa::pointf2 & texcoord) const {
	if (!colorTexture || !colorTexture->IsValid())
		return colorFactor;

	return colorFactor * (colorTexture->Sample(texcoord, Image::Mode::BILINEAR)).to_rgb();
}

float BSDF_FrostedGlass::GetRoughness(const Ubpa::pointf2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR)[0] * roughnessFactor;
}

float BSDF_FrostedGlass::GetAO(const Ubpa::pointf2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

void BSDF_FrostedGlass::ChangeNormal(const Ubpa::pointf2 & texcoord, const Ubpa::normalf & tangent, Ubpa::normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
	Ubpa::normalf tangentSpaceNormal = 2.f * rgb.cast_to<Ubpa::normalf>() - Ubpa::normalf(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
