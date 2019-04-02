#ifndef _ENGINE_MATERIAL_BSDF_MIRROR_H_
#define _ENGINE_MATERIAL_BSDF_MIRROR_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Mirror : public BSDF {
			ELE_SETUP(BSDF_Mirror)
		public:
			BSDF_Mirror(const RGBf & reflectance = RGBf(1.f)) : reflectance(reflectance) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) { return RGBf(0.f); };

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) { return 0; }

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD);

			virtual bool IsDelta() const { return true; }

		public:
			RGBf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_MIRROR_H_
