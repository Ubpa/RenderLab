#ifndef _ENGINE_MATERIAL_BSDF_GLASS_H_
#define _ENGINE_MATERIAL_BSDF_GLASS_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Glass : public BSDF {
			ELE_SETUP(BSDF_Glass)
		public:
			BSDF_Glass(float ior = 1.f, const RGBf & transmittance = RGBf(1.f), const RGBf & reflectance = RGBf(1.f))
				: ior(ior), transmittance(transmittance), reflectance(reflectance) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD);

			virtual bool IsDelta() const { return true; }

		public:
			float ior;
			RGBf transmittance;
			RGBf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_GLASS_H_
