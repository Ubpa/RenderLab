#pragma once

#include <Engine/Material/BSDF.h>

namespace Ubpa {
	class BSDF_Mirror : public BSDF {
	public:
		BSDF_Mirror(const rgbf& reflectance = rgbf(1.f)) : reflectance(reflectance) { }

	public:
		static const Ptr<BSDF_Mirror> New(const rgbf& reflectance = rgbf(1.f)) {
			return Ubpa::New<BSDF_Mirror>(reflectance);
		}

	protected:
		virtual ~BSDF_Mirror() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return rgbf(0.f); };

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return 0; }

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

		virtual bool IsDelta() const override { return true; }

	public:
		rgbf reflectance;
	};
}
