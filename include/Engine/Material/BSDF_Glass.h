#pragma once

#include <Engine/Material/BSDF.h>

namespace Ubpa {
	class BSDF_Glass : public BSDF {
	public:
		BSDF_Glass(float ior = 1.f, const rgbf& transmittance = rgbf(1.f), const rgbf& reflectance = rgbf(1.f))
			: ior(ior), transmittance(transmittance), reflectance(reflectance) { }

	public:
		static const Ptr<BSDF_Glass> New(float ior = 1.f, const rgbf& transmittance = rgbf(1.f), const rgbf& reflectance = rgbf(1.f)) {
			return Ubpa::New<BSDF_Glass>(ior, transmittance, reflectance);
		}

	protected:
		virtual ~BSDF_Glass() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return rgbf(0.f); }

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override { return 0; }

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

		virtual bool IsDelta() const override { return true; }

	public:
		float ior;
		rgbf transmittance;
		rgbf reflectance;
	};
}
