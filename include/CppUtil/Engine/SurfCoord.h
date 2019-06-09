#ifndef _CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_
#define _CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_

#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/Math.h>

namespace CppUtil {
	namespace Engine {
		namespace SurfCoord {
			inline float CosTheta(const Normalf & w) { return w.z; }
			inline float Cos2Theta(const Normalf & w) { return w.z * w.z; }
			inline float AbsCosTheta(const Normalf & w) { return std::abs(w.z); }
			inline float Sin2Theta(const Normalf & w) { return 1.f - Cos2Theta(w); }
			inline float SinTheta(const Normalf & w) { return std::sqrt(Sin2Theta(w)); }
			inline float TanTheta(const Normalf & w) { return SinTheta(w) / CosTheta(w); }
			inline float Tan2Theta(const Normalf & w) { return 1 / Cos2Theta(w) - 1; }
			inline float CosPhi(const Normalf & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 1 : Basic::Math::Clamp(w.x / sinTheta, -1, 1);
			}

			inline float SinPhi(const Normalf & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 0 : Basic::Math::Clamp(w.y / sinTheta, -1, 1);
			}

			inline float Cos2Phi(const Normalf & w) { return CosPhi(w) * CosPhi(w); }

			inline float Sin2Phi(const Normalf & w) { return SinPhi(w) * SinPhi(w); }

			inline float CosDPhi(const Normalf & lhs, const Normalf & rhs) {
				const float cosDPhi = (lhs.x*rhs.x + lhs.y*rhs.y) / ((lhs.x*lhs.x + lhs.y*lhs.y)*(rhs.x*rhs.x + rhs.y*rhs.y));
				return Basic::Math::Clamp(cosDPhi, -1, 1);
			}

			inline Normalf SphericalDirection(float sinTheta, float cosTheta, float phi) {
				return Normalf(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
			}

			inline bool IsVisible(const Normalf & w, const Normalf & wh) { return CosTheta(w) * w.Dot(wh) > 0; }

			inline bool IsEntering(const Normalf & wo, const Normalf & wh) { return wo.Dot(wh) > 0.f; }

			inline bool IsEntering(const Normalf & wo) { return CosTheta(wo) > 0.f; }

			inline bool IsSameSide(const Normalf & wo, const Normalf & wi) { return CosTheta(wo) * CosTheta(wi) > 0.f; }

			inline void SetOurward(Normalf & wh) { wh *= Basic::Math::sgn(CosTheta(wh)); }

			inline const Normalf Reflect(const Normalf & w) { return Normalf(-w.x, -w.y, w.z); }

			inline bool Refract(const Normalf & wo, Normalf & wi, float ior) {
				float inv = IsEntering(wo) ? 1.0f / ior : ior;

				float discriminant = 1 - (1 - CosTheta(wo) * CosTheta(wo)) * inv * inv;
				if (discriminant <= 0)
					return false;

				wi.x = -wo.x * inv;
				wi.y = -wo.y * inv;
				wi.z = -Basic::Math::sgn(wo.z) * sqrt(discriminant);
				wi.NormalizeSelf();

				return true;
			}

			inline bool IsOnSurf(const Normalf & w) {
				return Basic::Math::IsZero(CosTheta(w));
			}
		}
	}
}

#endif // !_CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_
