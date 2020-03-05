#pragma once

#include <Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_CookTorrance final : public BSDF {
		public:
			BSDF_CookTorrance(float ior = 1.5f, float m = 0.2f, const Ubpa::rgbf & refletance = Ubpa::rgbf(1.f), const Ubpa::rgbf & albedo = Ubpa::rgbf(1.f))
				: ior(ior), m(m), refletance(refletance) , albedo(albedo){ }

		public:
			static const Basic::Ptr<BSDF_CookTorrance> New(float ior = 1.5f, float m = 0.2f, const Ubpa::rgbf & refletance = Ubpa::rgbf(1.f), const Ubpa::rgbf & albedo = Ubpa::rgbf(1.f)) {
				return Basic::New<BSDF_CookTorrance>(ior, m, refletance, albedo);
			}

		protected:
			virtual ~BSDF_CookTorrance() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

		private:
			float NDF(const Ubpa::normalf & h);
			float Fr(const Ubpa::normalf & wi, const Ubpa::normalf & h);
			float G(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::normalf & h);

		public:
			float ior;
			float m;
			Ubpa::rgbf refletance;
			Ubpa::rgbf albedo;
		};
	}
}
