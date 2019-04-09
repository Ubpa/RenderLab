#ifndef _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_
#define _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_CookTorrance final : public BSDF {
		public:
			BSDF_CookTorrance(float ior = 1.5f, float m = 0.2f, const RGBf & refletance = RGBf(1.f), const RGBf & albedo = RGBf(1.f))
				: ior(ior), m(m), refletance(refletance) , albedo(albedo){ }

		public:
			static const Basic::Ptr<BSDF_CookTorrance> New(float ior = 1.5f, float m = 0.2f, const RGBf & refletance = RGBf(1.f), const RGBf & albedo = RGBf(1.f)) {
				return Basic::New<BSDF_CookTorrance>(ior, m, refletance, albedo);
			}

		protected:
			virtual ~BSDF_CookTorrance() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override;

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
