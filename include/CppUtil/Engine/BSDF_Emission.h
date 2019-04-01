#ifndef _ENGINE_MATERIAL_BSDF_EMISSION_H_
#define _ENGINE_MATERIAL_BSDF_EMISSION_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Emission : public BSDF {
			ELE_SETUP(BSDF_Emission)
		public:
			BSDF_Emission(const RGBf & color = RGBf(1.f), float intensity = 1.f)
				: color(color), intensity(intensity) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) { return RGBf(0.f); }

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) { return 0; }

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD) {
				PD = 0;
				return RGBf(0.f);
			}

			virtual const RGBf GetEmission() const { return intensity * color; }

		public:
			RGBf color;
			float intensity;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_EMISSION_H_
