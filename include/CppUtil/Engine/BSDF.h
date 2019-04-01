#ifndef _ENGINE_MATERIAL_BSDF_H_
#define _ENGINE_MATERIAL_BSDF_H_

#include <CppUtil/Engine/Material.h>
#include <CppUtil/Basic/Point2.h>
#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/RGB.h>

namespace CppUtil {
	namespace Engine {
		class BSDF : public Material {
			ELE_SETUP(BSDF)
		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) = 0;

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord) = 0;

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD) = 0;

			virtual const RGBf GetEmission() const { return RGBf(0.f); }

			virtual bool IsDelta() const { return false; }

			virtual void ChangeNormal(const Point2f & texcoord, const Normalf & tangent, Normalf & normal) const { return; };

		public:
			static const Normalf LocalReflect(const Normalf & w) {
				return Normalf(-w.x, -w.y, w.z);
			}
			static bool LocalRefract(const Normalf & wo, Normalf & wi, float ior);

		protected:
			static const Normalf TangentSpaceNormalToWorld(const Normalf & worldTangent, const Normalf & worldNormal, const Normalf & tangentSpaceNormal);
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_H_
