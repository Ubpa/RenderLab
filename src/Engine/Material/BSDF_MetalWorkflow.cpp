#include <Engine/BSDF_MetalWorkflow.h>

#include <Basic/CosineWeightedHemisphereSampler3D.h>

#include <Basic/Image.h>
#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const Ubpa::rgbf BSDF_MetalWorkflow::F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto roughness = GetRoughness(texcoord);
	//auto ao = GetAO(texcoord);

	sggx.SetAlpha(roughness);

	auto wh = (wo + wi).normalize();

	// Lambertian
	auto diffuse = albedo / Ubpa::PI<float>;

	auto D = sggx.D(wh);
	auto G = sggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	float denominator = 4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
	if (denominator == 0) // 极少发生。所以放在这里虽然性能不是最优，但逻辑顺畅些
		return Ubpa::rgbf(0.f);
	auto specular = D * G * F / denominator;

	auto kS = F;
	auto kD = (1 - metallic) * (Ubpa::rgbf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	return rst;
}

float BSDF_MetalWorkflow::PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) {
	auto roughness = GetRoughness(texcoord);
	sggx.SetAlpha(roughness);

	auto metallic = GetMetallic(texcoord);
	auto pSpecular = 1 / (2 - metallic);

	auto wh = wo + wi;
	if (wh.is_all_zero())
		return 0;
	
	wh.normalize_self();

	float pdDiffuse = SurfCoord::CosTheta(wi) * Math::INV_PI;
	float pdSpecular = sggx.PDF(wh) / (4.f*abs(wo.dot(wh))); // 根据几何关系以及前边的判 0 结果，这里无需判 0
	return Math::Lerp(pdDiffuse, pdSpecular, pSpecular);
}

const Ubpa::rgbf BSDF_MetalWorkflow::Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & pd) {
	auto roughness = GetRoughness(texcoord);
	sggx.SetAlpha(roughness);

	// 根据 metallic 来分别采样
	Ubpa::normalf wh;
	auto metallic = GetMetallic(texcoord);
	auto pSpecular = 1 / (2 - metallic);
	if (Math::Rand_F() < pSpecular) {
		wh = sggx.Sample_wh();
		wi = Ubpa::normalf::reflect(-wo, wh);
	}
	else {
		CosineWeightedHemisphereSampler3D sampler;
		wi = sampler.GetSample().cast_to<Ubpa::normalf>();
		wh = (wo + wi).normalize();
	}

	if (SurfCoord::CosTheta(wi) <= 0) {
		pd = 0;
		return Ubpa::rgbf(0.f);
	}

	float pdDiffuse = SurfCoord::CosTheta(wi) * Math::INV_PI;
	float pdSpecular = sggx.PDF(wh) / (4.f*abs(wo.dot(wh)));
	pd = Math::Lerp(pdDiffuse, pdSpecular, pSpecular);

	auto albedo = GetAlbedo(texcoord);
	auto diffuse = albedo * Math::INV_PI;

	auto D = sggx.D(wh);
	auto G = sggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	float denominator = 4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
	if (denominator == 0) {// 极少发生。所以放在这里虽然性能不是最优，但逻辑顺畅些
		pd = 0;
		wi = Ubpa::normalf(0.f);
		return Ubpa::rgbf(0.f);
	}
	
	auto specular = D * G * F / denominator;

	auto kS = F;
	auto kD = (1 - metallic) * (Ubpa::rgbf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	//auto ao = GetAO(texcoord);
	return rst;
}

const Ubpa::rgbf BSDF_MetalWorkflow::Fr(const Ubpa::normalf & w, const Ubpa::normalf & h, const Ubpa::rgbf & albedo, float metallic) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	const Ubpa::rgbf F0 = Ubpa::rgbf(0.04f).lerp(albedo, metallic);
	float HoWi = h.dot(w);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (Ubpa::rgbf(1.0f) - F0);
}

const Ubpa::rgbf BSDF_MetalWorkflow::GetAlbedo(const Ubpa::pointf2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
}

float BSDF_MetalWorkflow::GetMetallic(const Ubpa::pointf2 & texcoord) const {
	if (!metallicTexture || !metallicTexture->IsValid())
		return metallicFactor;

	return metallicFactor * metallicTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

float BSDF_MetalWorkflow::GetRoughness(const Ubpa::pointf2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessFactor * roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

float BSDF_MetalWorkflow::GetAO(const Ubpa::pointf2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

void BSDF_MetalWorkflow::ChangeNormal(const Ubpa::pointf2 & texcoord, const Ubpa::normalf & tangent, Ubpa::normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
	Ubpa::normalf tangentSpaceNormal = 2.f * rgb.cast_to<Ubpa::normalf>() - Ubpa::normalf(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
