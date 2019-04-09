#ifndef _ENGINE_MATERIAL_BSDF_MIRROR_H_
#define _ENGINE_MATERIAL_BSDF_MIRROR_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Mirror : public BSDF {
		public:
			BSDF_Mirror(const RGBf & reflectance = RGBf(1.f)) : reflectance(reflectance) { }

		public:
			static const Basic::Ptr<BSDF_Mirror> New(const RGBf & reflectance = RGBf(1.f)) {
				return Basic::New<BSDF_Mirror>(reflectance);
			}

		protected:
			virtual ~BSDF_Mirror() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return RGBf(0.f); };

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override;

			virtual bool IsDelta() const override { return true; }

		public:
			RGBf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_MIRROR_H_
