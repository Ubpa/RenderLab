#pragma once

#include <Engine/Material/BSDF.h>
#include <Engine/Material/GGX.h>

namespace Ubpa {
	class BSDF_Frostbite : public BSDF {
	public:
		BSDF_Frostbite(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f)
			: colorFactor(colorFactor), roughnessFactor(roughnessFactor), metallicFactor(metallicFactor),
			albedoTexture(nullptr), metallicTexture(nullptr), aoTexture(nullptr), roughnessTexture(nullptr), normalTexture(nullptr) { }

	public:
		static const Ptr<BSDF_Frostbite> New(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f) {
			return Ubpa::New<BSDF_Frostbite>(colorFactor, roughnessFactor, metallicFactor);
		}

	protected:
		virtual ~BSDF_Frostbite() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

		virtual void ChangeNormal(const pointf2& texcoord, const normalf& tangent, normalf& normal) const override;

	private:
		// Fresnel
		static const rgbf Fr(const normalf& w, const normalf& h, const rgbf& albedo, float metallic);

		const rgbf GetAlbedo(const pointf2& texcoord) const;
		float GetMetallic(const pointf2& texcoord) const;
		float GetRoughness(const pointf2& texcoord) const;
		float GetAO(const pointf2& texcoord) const;

		static const float Fr_DisneyDiffuse(const normalf& wo, const normalf& wi, float linearRoughness);

	public:
		GGX ggx;

		rgbf colorFactor;
		Ptr<Image> albedoTexture;

		// 0--1
		float metallicFactor;
		Ptr<Image> metallicTexture;

		// 0--1
		float roughnessFactor;
		Ptr<Image> roughnessTexture;

		Ptr<Image> aoTexture; // 只用与实时渲染

		Ptr<Image> normalTexture;
	};
}
