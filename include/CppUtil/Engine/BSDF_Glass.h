#ifndef _ENGINE_MATERIAL_BSDF_GLASS_H_
#define _ENGINE_MATERIAL_BSDF_GLASS_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Glass : public BSDF {
		public:
			BSDF_Glass(float ior = 1.f, const RGBf & transmittance = RGBf(1.f), const RGBf & reflectance = RGBf(1.f))
				: ior(ior), transmittance(transmittance), reflectance(reflectance) { }

		public:
			static const Basic::Ptr<BSDF_Glass> New(float ior = 1.f, const RGBf & transmittance = RGBf(1.f), const RGBf & reflectance = RGBf(1.f)) {
				return Basic::New<BSDF_Glass>(ior, transmittance, reflectance);
			}

		protected:
			virtual ~BSDF_Glass() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return RGBf(0.f); }

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override;

			virtual bool IsDelta() const override { return true; }

		public:
			float ior;
			RGBf transmittance;
			RGBf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_GLASS_H_
