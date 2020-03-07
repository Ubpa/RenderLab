#include <Engine/Material/BSDF_MetalWorkflow.h>

#include <Basic/Sampler/CosHsSampler3D.h>

#include <Basic/Image.h>
#include <Basic/Math.h>

using namespace Ubpa;

const rgbf BSDF_MetalWorkflow::F(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto roughness = GetRoughness(texcoord);
	//auto ao = GetAO(texcoord);

	sggx.SetAlpha(roughness);

	auto wh = (wo + wi).normalize();

	// Lambertian
	auto diffuse = albedo / PI<float>;

	auto D = sggx.D(wh);
	auto G = sggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	float denominator = 4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi));
	if (denominator == 0) // 极少发生。所以放在这里虽然性能不是最优，但逻辑顺畅些
		return rgbf(0.f);
	auto specular = D * G * F / denominator;

	auto kS = F;
	auto kD = (1 - metallic) * (rgbf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	return rst;
}

float BSDF_MetalWorkflow::PDF(const normalf & wo, const normalf & wi, const pointf2 & texcoord) {
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

const rgbf BSDF_MetalWorkflow::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & pd) {
	auto roughness = GetRoughness(texcoord);
	sggx.SetAlpha(roughness);

	// 根据 metallic 来分别采样
	normalf wh;
	auto metallic = GetMetallic(texcoord);
	auto pSpecular = 1 / (2 - metallic);
	if (Math::Rand_F() < pSpecular) {
		wh = sggx.Sample_wh();
		wi = normalf::reflect(-wo, wh);
	}
	else {
		CosHsSampler3D sampler;
		wi = sampler.GetSample().cast_to<normalf>();
		wh = (wo + wi).normalize();
	}

	if (SurfCoord::CosTheta(wi) <= 0) {
		pd = 0;
		return rgbf(0.f);
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
		wi = normalf(0.f);
		return rgbf(0.f);
	}
	
	auto specular = D * G * F / denominator;

	auto kS = F;
	auto kD = (1 - metallic) * (rgbf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	//auto ao = GetAO(texcoord);
	return rst;
}

const rgbf BSDF_MetalWorkflow::Fr(const normalf & w, const normalf & h, const rgbf & albedo, float metallic) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	const rgbf F0 = rgbf(0.04f).lerp(albedo, metallic);
	float HoWi = h.dot(w);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (rgbf(1.0f) - F0);
}

const rgbf BSDF_MetalWorkflow::GetAlbedo(const pointf2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
}

float BSDF_MetalWorkflow::GetMetallic(const pointf2 & texcoord) const {
	if (!metallicTexture || !metallicTexture->IsValid())
		return metallicFactor;

	return metallicFactor * metallicTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

float BSDF_MetalWorkflow::GetRoughness(const pointf2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessFactor * roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

float BSDF_MetalWorkflow::GetAO(const pointf2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR)[0];
}

void BSDF_MetalWorkflow::ChangeNormal(const pointf2 & texcoord, const normalf & tangent, normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).to_rgb();
	normalf tangentSpaceNormal = 2.f * rgb.cast_to<normalf>() - normalf(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
