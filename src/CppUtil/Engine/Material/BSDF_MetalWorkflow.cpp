#include <CppUtil/Engine/BSDF_MetalWorkflow.h>

#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const RGBf BSDF_MetalWorkflow::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto roughness = GetRoughness(texcoord);
	//auto ao = GetAO(texcoord);

	sggx.SetAlpha(roughness);

	auto wh = (wo + wi).Normalize();

	auto diffuse = albedo / Math::PI;

	auto D = sggx.D(wh);
	auto G = sggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	auto specular = D * G * F / (4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));

	auto kS = F;
	auto kD = (1 - metallic) * (RGBf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	//return ao * rst;
	return rst;
}

float BSDF_MetalWorkflow::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto roughness = GetRoughness(texcoord);
	sggx.SetAlpha(roughness);

	auto metallic = GetMetallic(texcoord);

	const auto wh = (wo + wi).Normalize();

	float pdDiffuse = SurfCoord::CosTheta(wh) * Math::INV_PI;
	float pdSpecular = sggx.PDF(wh) / (4.f*abs(wo.Dot(wh)));
	return Math::Lerp(pdDiffuse, pdSpecular, metallic);
}

const RGBf BSDF_MetalWorkflow::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & pd) {
	auto roughness = GetRoughness(texcoord);
	sggx.SetAlpha(roughness);

	// 根据 metallic 来分别采样
	Normalf wh;
	auto metallic = GetMetallic(texcoord);
	if (Math::Rand_F() < metallic)
		wh = sggx.Sample_wh();
	else {
		CosineWeightedHemisphereSampler3D sampler;
		wh = sampler.GetSample();
	}

	wi = Normalf::Reflect(-wo, wh);
	if (SurfCoord::CosTheta(wi) <= 0) {
		pd = 0;
		return RGBf(0.f);
	}

	float pdDiffuse = SurfCoord::CosTheta(wh) * Math::INV_PI;
	float pdSpecular = sggx.PDF(wh) / (4.f*abs(wo.Dot(wh)));
	pd = Math::Lerp(pdDiffuse, pdSpecular, metallic);

	auto albedo = GetAlbedo(texcoord);
	auto diffuse = albedo / Math::PI;

	auto D = sggx.D(wh);
	auto G = sggx.G(wo, wi, wh);
	auto F = Fr(wo, wh, albedo, metallic);
	auto specular = D * G * F / (4.f * abs(SurfCoord::CosTheta(wo) * SurfCoord::CosTheta(wi)));

	auto kS = F;
	auto kD = (1 - metallic) * (RGBf(1.f) - kS);

	auto rst = kD * diffuse + specular;

	//auto ao = GetAO(texcoord);
	return rst;
}

const RGBf BSDF_MetalWorkflow::Fr(const Normalf & w, const Normalf & h, const RGBf & albedo, float metallic) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	const RGBf F0 = RGBf(0.04f).LerpWith(albedo, metallic);
	float HoWi = h.Dot(w);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (RGBf(1.0f) - F0);
}

const RGBf BSDF_MetalWorkflow::GetAlbedo(const Point2 & texcoord) const {
	if (!albedoTexture || !albedoTexture->IsValid())
		return colorFactor;

	return colorFactor * albedoTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
}

float BSDF_MetalWorkflow::GetMetallic(const Point2 & texcoord) const {
	if (!metallicTexture || !metallicTexture->IsValid())
		return metallicFactor;

	return metallicFactor * metallicTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

float BSDF_MetalWorkflow::GetRoughness(const Point2 & texcoord) const {
	if (!roughnessTexture || !roughnessTexture->IsValid())
		return roughnessFactor;

	return roughnessFactor * roughnessTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

float BSDF_MetalWorkflow::GetAO(const Point2 & texcoord) const {
	if (!aoTexture || !aoTexture->IsValid())
		return 1.0f;

	return aoTexture->Sample(texcoord, Image::Mode::BILINEAR).r;
}

void BSDF_MetalWorkflow::ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const {
	if (!normalTexture || !normalTexture->IsValid())
		return;

	const auto rgb = normalTexture->Sample(texcoord, Image::Mode::BILINEAR).ToRGB();
	Normalf tangentSpaceNormal = 2.f * Vec3(rgb.r, rgb.g, rgb.b) - Vec3(1.f);

	normal = TangentSpaceNormalToWorld(tangent, normal, tangentSpaceNormal);
}
