#pragma once

#include <Engine/Material/BSDF.h>

namespace Ubpa {
	class BSDF_Emission final : public BSDF {
	public:
		BSDF_Emission(const rgbf& color = rgbf(1.f), float intensity = 1.f)
			: color(color), intensity(intensity) { }

	public:
		static const Ptr<BSDF_Emission> New(const rgbf& color = rgbf(1.f), float intensity = 1.f) {
			return Ubpa::New<BSDF_Emission>(color, intensity);
		}

	protected:
		virtual ~BSDF_Emission() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return rgbf(0.f); }

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return 0; }

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override {
			PD = 0;
			return rgbf(0.f);
		}

		// luminance
		virtual const rgbf Emission(const normalf& wo) const override { return SurfCoord::CosTheta(wo) > 0 ? intensity * color : rgbf(0.f); }

	public:
		rgbf color;
		float intensity;
	};
}
