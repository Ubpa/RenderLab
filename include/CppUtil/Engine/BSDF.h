#ifndef _ENGINE_MATERIAL_BSDF_H_
#define _ENGINE_MATERIAL_BSDF_H_

#include <CppUtil/Engine/Material.h>
#include <CppUtil/Basic/UGM/Point2.h>
#include <CppUtil/Basic/UGM/Vector.h>
#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Engine {
		class BSDF : public Material {
		protected:
			BSDF() = default;
			virtual ~BSDF() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) = 0;

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) = 0;

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) = 0;

			virtual const RGBf GetEmission() const { return RGBf(0.f); }

			virtual bool IsDelta() const { return false; }

			virtual void ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const { return; };

		public:
			static const Normalf LocalReflect(const Normalf & w) {
				return Normalf(-w.x, -w.y, w.z);
			}
			static bool LocalRefract(const Normalf & wo, Normalf & wi, float ior);

			static float CosTheta(const Vec3 & w) { return w.z; }
			static float Cos2Theta(const Vec3 & w) { return w.z * w.z; }
			static float AbsCosTheta(const Vec3 & w) { return std::abs(w.z); }
			static float Sin2Theta(const Vec3 & w) { return 1.f - Cos2Theta(w); }
			static float SinTheta(const Vec3 & w) { return std::sqrt(Sin2Theta(w)); }
			static float TanTheta(const Vec3 & w) { return SinTheta(w) / CosTheta(w); }
			static float Tan2Theta(const Vec3 & w) { return Sin2Theta(w) / Cos2Theta(w); }
			static float CosPhi(const Vec3 & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 1 : Basic::Math::Clamp(w.x / sinTheta, -1, 1);
			}
			static float SinPhi(const Vec3 & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 0 : Basic::Math::Clamp(w.y / sinTheta, -1, 1);
			}
			static float Cos2Phi(const Vec3 & w) {
				return CosPhi(w) * CosPhi(w);
			}
			static float Sin2Phi(const Vec3 & w) {
				return SinPhi(w) * SinPhi(w);
			}
			static float CosDPhi(const Vec3 & lhs, const Vec3 & rhs) {
				const float cosDPhi = (lhs.x*rhs.x + lhs.y*rhs.y) / ((lhs.x*lhs.x + lhs.y*lhs.y)*(rhs.x*rhs.x+rhs.y*rhs.y));
				return Basic::Math::Clamp(cosDPhi, -1, 1);
			}

		protected:
			static const Normalf TangentSpaceNormalToWorld(const Normalf & worldTangent, const Normalf & worldNormal, const Normalf & tangentSpaceNormal);
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_H_
