#pragma once

#include <Engine/Material/BSDF.h>
#include <Engine/Material/GGX.h>

namespace Ubpa {
	class BSDF_FrostedGlass : public BSDF {
	public:
		BSDF_FrostedGlass(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float ior = 1.5f)
			: colorFactor(colorFactor), roughnessFactor(roughnessFactor), ior(ior),
			colorTexture(nullptr),
			roughnessTexture(nullptr),
			aoTexture(nullptr),
			normalTexture(nullptr) { }

	public:
		static const Ptr<BSDF_FrostedGlass> New(const rgbf& colorFactor = rgbf(1.f), float roughnessFactor = 1.f, float ior = 1.5f) {
			return Ubpa::New<BSDF_FrostedGlass>(colorFactor, roughnessFactor, ior);
		}

	protected:
		virtual ~BSDF_FrostedGlass() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

		virtual void ChangeNormal(const pointf2& texcoord, const normalf& tangent, normalf& normal) const override;

	private:
		static float Fr(const normalf& v, const normalf& h, float ior);

	private:
		const rgbf GetColor(const pointf2& texcoord) const;
		float GetRoughness(const pointf2& texcoord) const;
		float GetAO(const pointf2& texcoord) const;

	public:
		GGX ggx;

		rgbf colorFactor;
		Ptr<Image> colorTexture;

		float roughnessFactor;
		Ptr<Image> roughnessTexture;

		Ptr<Image> aoTexture;
		Ptr<Image> normalTexture;

		float ior;
	};
}
