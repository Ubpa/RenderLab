#pragma once

#include <Engine/Material/BSDF.h>

namespace Ubpa {
	class BSDF_CookTorrance final : public BSDF {
	public:
		BSDF_CookTorrance(float ior = 1.5f, float m = 0.2f, const rgbf& refletance = rgbf(1.f), const rgbf& albedo = rgbf(1.f))
			: ior(ior), m(m), refletance(refletance), albedo(albedo) { }

	public:
		static const Ptr<BSDF_CookTorrance> New(float ior = 1.5f, float m = 0.2f, const rgbf& refletance = rgbf(1.f), const rgbf& albedo = rgbf(1.f)) {
			return Ubpa::New<BSDF_CookTorrance>(ior, m, refletance, albedo);
		}

	protected:
		virtual ~BSDF_CookTorrance() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) override;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) override;

	private:
		float NDF(const normalf& h);
		float Fr(const normalf& wi, const normalf& h);
		float G(const normalf& wo, const normalf& wi, const normalf& h);

	public:
		float ior;
		float m;
		rgbf refletance;
		rgbf albedo;
	};
}
