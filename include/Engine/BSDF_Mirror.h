#ifndef _ENGINE_MATERIAL_BSDF_MIRROR_H_
#define _ENGINE_MATERIAL_BSDF_MIRROR_H_

#include <Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Mirror : public BSDF {
		public:
			BSDF_Mirror(const Ubpa::rgbf & reflectance = Ubpa::rgbf(1.f)) : reflectance(reflectance) { }

		public:
			static const Basic::Ptr<BSDF_Mirror> New(const Ubpa::rgbf & reflectance = Ubpa::rgbf(1.f)) {
				return Basic::New<BSDF_Mirror>(reflectance);
			}

		protected:
			virtual ~BSDF_Mirror() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return Ubpa::rgbf(0.f); };

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override { return 0; }

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

			virtual bool IsDelta() const override { return true; }

		public:
			Ubpa::rgbf reflectance;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_MIRROR_H_
