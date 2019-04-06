#include <CppUtil/Engine/BSDF_MetalWorkflow.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const RGBf BSDF_MetalWorkflow::F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto roughness = GetRoughness(texcoord);
	auto ao = GetAO(texcoord);

	auto h = (wo + wi).Normalize();
	auto diffuse = albedo / Math::PI;
	auto fr = Fr(wi, h, albedo, metallic);

	return ao * ((1 - metallic)*(RGBf(1.0f) - fr)*diffuse + MS_BRDF(wo, wi, fr, albedo, roughness));
}

float BSDF_MetalWorkflow::PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) {
	auto roughness = GetRoughness(texcoord);

	const auto h = (wo + wi).Normalize();
	return NDF(h, roughness) / 4.0f;
}

const RGBf BSDF_MetalWorkflow::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & pd) {
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();
	auto roughness = GetRoughness(texcoord);

	// 根据 NDF 采样
	float alpha = roughness * roughness;
	float cosTheta2 = (1 - Xi1) / (Xi1*(alpha*alpha - 1) + 1);
	float cosTheta = sqrt(cosTheta2);
	float sinTheta = sqrt(1 - cosTheta2);
	float phi = 2 * Math::PI*Xi2;
	const Normalf h(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	wi = Normalf::Reflect(-wo, h);
	if (wi.z <= 0) {
		pd = 0;
		return RGBf(0.f);
	}
	pd = NDF(h, roughness) / 4.0f;

	/*
	// 均匀采样
	float cosTheta = Xi1;
	float sinTheta = sqrt(1 - cosTheta * cosTheta);
	float phi = 2 * Math::PI * Xi2;
	wi = vec3(sinTheta*cos(phi), sinTheta*sin(phi), cosTheta);
	vec3 h = normalize(wo + wi);
	pd = 1.0f / (2.0f * Math::PI);
	*/

	// sample
	auto albedo = GetAlbedo(texcoord);
	auto metallic = GetMetallic(texcoord);
	auto ao = GetAO(texcoord);

	auto fr = Fr(wi, h, albedo, metallic);
	auto diffuse = albedo / Math::PI;

	return ao * ((1 - metallic)*(RGBf(1.f) - fr)*diffuse + MS_BRDF(wo, wi, fr, albedo, roughness));
}

const RGBf BSDF_MetalWorkflow::MS_BRDF(const Normalf & wo, const Normalf & wi, const RGBf & albedo, float metallic, float roughness) {
	const auto h = (wo + wi).Normalize();
	return NDF(h, roughness)*Fr(wi, h, albedo, metallic)*G(wo, wi, roughness) / (4 * wo.z*wi.z);
}

const RGBf BSDF_MetalWorkflow::MS_BRDF(const Normalf & wo, const Normalf & wi, const RGBf & fr, const RGBf & albedo, float roughness) {
	const auto h = (wo + wi).Normalize();
	return NDF(h, roughness) * G(wo, wi, roughness) / (4 * wo.z* wi.z) * fr;
}

float BSDF_MetalWorkflow::NDF(const Normalf & h, float roughness) {
	//  GGX/Trowbridge-Reitz

	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NoH = h.z;
	return alpha2 / (Math::PI*pow(NoH*NoH*(alpha2 - 1) + 1, 2));
}

const RGBf BSDF_MetalWorkflow::Fr(const Normalf & wi, const Normalf & h, const RGBf & albedo, float metallic) {
	// Schlick’s approximation
	// use a Spherical Gaussian approximation to replace the power.
	//  slightly more efficient to calculate and the difference is imperceptible

	const RGBf F0 = RGBf(0.04f).LerpWith(albedo, metallic);
	float HoWi = h.Dot(wi);
	return F0 + pow(2.0f, (-5.55473f * HoWi - 6.98316f) * HoWi) * (RGBf(1.0f) - F0);
}

float BSDF_MetalWorkflow::G(const Normalf & wo, const Normalf & wi, float roughness) {
	// Schlick, remap roughness and k

	// k = alpha / 2
	// direct light: alpha = pow( (roughness + 1) / 2, 2)
	// IBL(image base lighting) : alpha = pow( roughness, 2)

	if (wo.z <= 0 || wi.z <= 0)
		return 0;

	float k = pow(roughness + 1.f, 2) / 8.f;
	float one_minus_k = 1.f - k;

	float G1_wo = wo.z / (wo.z * one_minus_k + k);
	float G1_wi = wi.z / (wi.z * one_minus_k + k);

	return G1_wo * G1_wi;
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
