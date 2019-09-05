#ifndef _ENGINE_MATERIAL_BSDF_EMISSION_H_
#define _ENGINE_MATERIAL_BSDF_EMISSION_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Emission final : public BSDF {
		public:
			BSDF_Emission(const RGBf & color = RGBf(1.f), float intensity = 1.f)
				: color(color), intensity(intensity) { }

		public:
			static const Basic::Ptr<BSDF_Emission> New(const RGBf & color = RGBf(1.f), float intensity = 1.f) {
				return Basic::New<BSDF_Emission>(color, intensity);
			}

		protected:
			virtual ~BSDF_Emission() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return RGBf(0.f); }

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override {
				PD = 0;
				return RGBf(0.f);
			}

			// luminance
			virtual const RGBf Emission(const Normalf & wo) const override { return SurfCoord::CosTheta(wo) > 0 ? intensity * color : RGBf(0); }

		public:
			RGBf color;
			float intensity;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_EMISSION_H_
