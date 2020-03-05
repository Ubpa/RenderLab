#ifndef _ENGINE_MATERIAL_BSDF_GLASS_H_
#define _ENGINE_MATERIAL_BSDF_GLASS_H_

#include <Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Glass : public BSDF {
		public:
			BSDF_Glass(float ior = 1.f, const Ubpa::rgbf & transmittance = Ubpa::rgbf(1.f), const Ubpa::rgbf & reflectance = Ubpa::rgbf(1.f))
				: ior(ior), transmittance(transmittance), reflectance(reflectance) { }

		public:
			static const Basic::Ptr<BSDF_Glass> New(float ior = 1.f, const Ubpa::rgbf & transmittance = Ubpa::rgbf(1.f), const Ubpa::rgbf & reflectance = Ubpa::rgbf(1.f)) {
				return Basic::New<BSDF_Glass>(ior, transmittance, reflectance);
			}

		protected:
			virtual ~BSDF_Glass() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return Ubpa::rgbf(0.f); }

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

			virtual bool IsDelta() const override { return true; }

		public:
			float ior;
			Ubpa::rgbf transmittance;
			Ubpa::rgbf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_GLASS_H_
