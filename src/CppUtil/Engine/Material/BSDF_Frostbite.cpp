#include <CppUtil/Engine/BSDF_Frostbite.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf BSDF_Frostbite::Fr(const Normalf & w, const Normalf & h, const RGBf & albedo, float metallic) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	const RGBf F0 = RGBf(0.04f).LerpWith(albedo, metallic);
	float HoWi = h.Dot(w);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (RGBf(1.0f) - F0);
}

const Vec3f BSDF_Frostbite::Fr_DisneyDiffuse(const Normalf & wo, const Normalf & wi, float linearRoughness) {
	auto h = (wo + wi).Normalize();
	float HoWi = h.Dot(wi);
	float HoWi2 = HoWi * HoWi;
	float HoWo = h.Dot(wo);
	float HoWo2 = HoWo * HoWo;

	float energyBias = Math::Lerp(0.f, 0.5f, linearRoughness);
	float energyFactor = Math::Lerp(1.f, 1.f / 1.51f, linearRoughness);
	float fd90 = energyBias + 2.f * HoWi2 * linearRoughness;
	float lightScatter = 1.f + fd90 * pow(1.f - HoWi2, 5);
	float viewScatter = 1.f + fd90 * pow(1.f - HoWo2, 5);

	return lightScatter * viewScatter * energyFactor;
}

const RGBf BSDF_Frostbite::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto roughness = GetRoughness(texcoord);
	//auto ao = GetAO(texcoord);

	ggx.SetAlpha(roughness);

	auto wh = (wo + wi).Normalize();

	// renormalized Disney
	auto diffuse = albedo / Math::PI * Fr_DisneyDiffuse(wo, wi, roughness);

	auto D = ggx.D(wh);
	auto G = ggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	float denominator = 4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
	if (denominator == 0) // 极少发生。所以放在这里虽然性能不是最优，但逻辑顺畅些
		return RGBf(0.f);
	auto specular = D * G * F / denominator;

	auto rst = (1 - metallic) * diffuse + specular;

	return rst;
}

// probability density function
float BSDF_Frostbite::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	auto metallic = GetMetallic(texcoord);
	auto pSpecular = 1 / (2 - metallic);

	auto wh = wo + wi;
	if (wh.IsZero())
		return 0;

	wh.NormalizeSelf();

	float pdDiffuse = SurfCoord::CosTheta(wi) * Math::INV_PI;
	float pdSpecular = ggx.PDF(wh) / (4.f*abs(wo.Dot(wh))); // 根据几何关系以及前边的判 0 结果，这里无需判 0
	return Math::Lerp(pdDiffuse, pdSpecular, pSpecular);
}

// PD is probability density
// return albedo
const RGBf BSDF_Frostbite::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) {
	auto roughness = GetRoughness(texcoord);
	ggx.SetAlpha(roughness);

	// 根据 metallic 来分别采样
	Normalf wh;
	auto metallic = GetMetallic(texcoord);
	auto pSpecular = 1 / (2 - metallic);
	if (Math::Rand_F() < pSpecular) {
		wh = ggx.Sample_wh();
		wi = Normalf::Reflect(-wo, wh);
	}
	else {
		CosineWeightedHemisphereSampler3D sampler;
		wi = sampler.GetSample();
		wh = (wo + wi).Normalize();
	}

	if (SurfCoord::CosTheta(wi) <= 0) {
		PD = 0;
		return RGBf(0.f);
	}

	float pdDiffuse = SurfCoord::CosTheta(wi) * Math::INV_PI;
	float pdSpecular = ggx.PDF(wh) / (4.f*abs(wo.Dot(wh)));
	PD = Math::Lerp(pdDiffuse, pdSpecular, pSpecular);

	auto albedo = GetAlbedo(texcoord);
	auto diffuse = albedo * Math::INV_PI * Fr_DisneyDiffuse(wo, wi, roughness);

	auto D = ggx.D(wh);
	auto G = ggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	float denominator = 4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
	if (denominator == 0) {// 极少发生。所以放在这里虽然性能不是最优，但逻辑顺畅些
		PD = 0;
		wi = Normalf(0.f);
		return RGBf(0.f);
	}

	auto specular = D * G * F / denominator;

	auto kS = F;
	auto kD = (1 - metallic) * (RGBf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	//auto ao = GetAO(texcoord);
	return rst;
}

void BSDF_Frostbite::ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
	Normalf tangentSpaceNormal = 2.f * Vec3(rgb.r, rgb.g, rgb.b) - Vec3(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}

const RGBf BSDF_Frostbite::GetAlbedo(const Point2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
}

float BSDF_Frostbite::GetMetallic(const Point2 & texcoord) const {
	if (!metallicTexture || !metallicTexture->IsValid())
		return metallicFactor;

	return metallicFactor * metallicTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

float BSDF_Frostbite::GetRoughness(const Point2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessFactor * roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

float BSDF_Frostbite::GetAO(const Point2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}