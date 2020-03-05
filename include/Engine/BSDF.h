#ifndef _ENGINE_MATERIAL_BSDF_H_
#define _ENGINE_MATERIAL_BSDF_H_

#include <Engine/Material.h>
#include <Engine/SurfCoord.h>

namespace CppUtil {
	namespace Engine {
		class BSDF : public Material {
		protected:
			BSDF() = default;
			virtual ~BSDF() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) = 0;

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) = 0;

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) = 0;

			virtual bool IsDelta() const { return false; }

			virtual void ChangeNormal(const Ubpa::pointf2 & texcoord, const Ubpa::normalf & tangent, Ubpa::normalf & normal) const { return; };

			// Luminance
			virtual const Ubpa::rgbf Emission(const Ubpa::normalf & wo) const { return Ubpa::rgbf(0.f); }

		protected:
			static const Ubpa::normalf TangentSpaceNormalToWorld(const Ubpa::normalf & worldTangent, const Ubpa::normalf & worldNormal, const Ubpa::normalf & tangentSpaceNormal);
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_H_
