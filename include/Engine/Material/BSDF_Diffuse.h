#pragma once

#include <Engine/Material/BSDF.h>

#include <Basic/Sampler/CosHsSampler3D.h>

namespace Ubpa {
	class BSDF_Diffuse final : public BSDF {
	public:
		BSDF_Diffuse(const rgbf& colorFactor = rgbf(1.f))
			: colorFactor(colorFactor), albedoTexture(nullptr) { }

	public:
		static const Ptr<BSDF_Diffuse> New(const rgbf& colorFactor = rgbf(1.f)) {
			return Ubpa::New<BSDF_Diffuse>(colorFactor);
		}

	protected:
		virtual ~BSDF_Diffuse() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

	private:
		const rgbf GetAlbedo(const pointf2& texcoord) const;

	public:
		rgbf colorFactor;
		Ptr<Image> albedoTexture;

	private:
		CosHsSampler3D sampler;
	};
}
