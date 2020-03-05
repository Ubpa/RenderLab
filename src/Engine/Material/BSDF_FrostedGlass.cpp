#include <Engine/BSDF_FrostedGlass.h>

#include <Basic/Math.h>
#include <Basic/Image.h>

using namespace Ubpa;

using namespace std;

float BSDF_FrostedGlass::Fr(const normalf & v, const normalf & h, float ior) {
	// 需要是低折射率介质中的角
	float cosTheta = v.dot(h);
	if (!SurfCoord::IsEntering(v,h)) {
		normalf vInLowIOR = normalf::refract(-v, -h, ior);
		if (vInLowIOR.is_all_zero())
			return 1.f;

		cosTheta = vInLowIOR.dot(h);
	}

	float R0 = pow((ior - 1) / (ior + 1), 2);

	return R0 + (1 - R0) * pow((1 - cosTheta), 5);
}

const rgbf BSDF_FrostedGlass::F(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	if (SurfCoord::CosTheta(wo) == 0 || SurfCoord::CosTheta(wi) == 0)
		return rgbf(0.f);

	auto color = GetColor(texcoord);
	color *= GetAO(texcoord);

	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	bool isReflect = SurfCoord::IsSameSide(wo, wi);
	if (isReflect) {
		normalf h = (wo + wi).normalize();
		SurfCoord::SetOurward(h);

		float bsdfVal = Fr(wo, h, ior) * ggx.D(h) * ggx.G_Smith(wo,wi,h) / abs(4.f * SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
		return bsdfVal * color;
	}
	else {
		float etai = 1.f, etat = ior;
		if (!SurfCoord::IsEntering(wo))
			swap(etai, etat);

		normalf h = -(etai * wo + etat * wi).normalize();
		if (SurfCoord::IsOnSurf(h))
			return rgbf(0.f);

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
float BSDF_FrostedGlass::PDF(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	normalf h;
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

const rgbf BSDF_FrostedGlass::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & PD) {
	float roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	auto h = ggx.Sample_wh();

	float fr = Fr(wo, h, ior);
	bool isReflect = Math::Rand_F() < fr;

	if (isReflect) {
		wi = normalf::reflect(-wo, h);
		if (!SurfCoord::IsSameSide(wo,wi)) {
			PD = 0;
			return rgbf(0.f);
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
			wi = normalf::refract(-wo, h, etai / etat);
		}
		else {
			swap(etai, etat);
			wi = normalf::refract(-wo, -h, etai / etat);
		}

		if (SurfCoord::IsSameSide(wi,wo) || wi.is_all_zero()) {
			PD = 0;
			return rgbf(0.f);
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

const rgbf BSDF_FrostedGlass::GetColor(const pointf2 & texcoord) const {
	if (!colorTexture || !colorTexture->IsValid())
		return colorFactor;

	return colorFactor * (colorTexture->Sample(texcoord, Image::Mode::BILINEAR)).to_rgb();
}

float BSDF_FrostedGlass::GetRoughness(const pointf2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR)[0] * roughnessFactor;
}

float BSDF_FrostedGlass::GetAO(const pointf2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

void BSDF_FrostedGlass::ChangeNormal(const pointf2 & texcoord, const normalf & tangent, normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
	normalf tangentSpaceNormal = 2.f * rgb.cast_to<normalf>() - normalf(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
