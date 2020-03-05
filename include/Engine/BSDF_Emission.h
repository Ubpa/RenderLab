#pragma once

#include <Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Emission final : public BSDF {
		public:
			BSDF_Emission(const Ubpa::rgbf & color = Ubpa::rgbf(1.f), float intensity = 1.f)
				: color(color), intensity(intensity) { }

		public:
			static const Basic::Ptr<BSDF_Emission> New(const Ubpa::rgbf & color = Ubpa::rgbf(1.f), float intensity = 1.f) {
				return Basic::New<BSDF_Emission>(color, intensity);
			}

		protected:
			virtual ~BSDF_Emission() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return Ubpa::rgbf(0.f); }

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override {
				PD = 0;
				return Ubpa::rgbf(0.f);
			}

			// luminance
			virtual const Ubpa::rgbf Emission(const Ubpa::normalf & wo) const override { return SurfCoord::CosTheta(wo) > 0 ? intensity * color : Ubpa::rgbf(0.f); }

		public:
			Ubpa::rgbf color;
			float intensity;
		};
	}
}
