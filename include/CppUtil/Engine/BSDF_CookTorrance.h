#ifndef _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_
#define _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_CookTorrance : public BSDF {
			ELE_SETUP(BSDF_CookTorrance)
		public:
			BSDF_CookTorrance(float ior, float m, const RGBf & refletance = RGBf(1.f), const RGBf & albedo = RGBf(1.f))
				: ior(ior), m(m), refletance(refletance) , albedo(albedo){ }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord);

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord);

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD);

		private:
			float NDF(const Normalf & h);
			float Fr(const Normalf & wi, const Normalf & h);
			float G(const Normalf & wo, const Normalf & wi, const Normalf & h);

		public:
			float ior;
			float m;
			RGBf refletance;
			RGBf albedo;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_
