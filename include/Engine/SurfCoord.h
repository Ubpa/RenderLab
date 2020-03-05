#ifndef _CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_
#define _CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_

#include <UGM/normal.h>
#include <Basic/Math.h>

namespace CppUtil {
	namespace Engine {
		namespace SurfCoord {
			inline float CosTheta(const Ubpa::normalf & w) { return w[2]; }
			inline float Cos2Theta(const Ubpa::normalf & w) { return w[2] * w[2]; }
			inline float AbsCosTheta(const Ubpa::normalf & w) { return std::abs(w[2]); }
			inline float Sin2Theta(const Ubpa::normalf & w) { return 1.f - Cos2Theta(w); }
			inline float SinTheta(const Ubpa::normalf & w) { return std::sqrt(Sin2Theta(w)); }
			inline float TanTheta(const Ubpa::normalf & w) { return SinTheta(w) / CosTheta(w); }
			inline float Tan2Theta(const Ubpa::normalf & w) { return 1 / Cos2Theta(w) - 1; }
			inline float CosPhi(const Ubpa::normalf & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 1 : Basic::Math::Clamp(w[0] / sinTheta, -1, 1);
			}

			inline float SinPhi(const Ubpa::normalf & w) {
				float sinTheta = SinTheta(w);
				return sinTheta == 0 ? 0 : Basic::Math::Clamp(w[1] / sinTheta, -1, 1);
			}

			inline float Cos2Phi(const Ubpa::normalf & w) { return CosPhi(w) * CosPhi(w); }

			inline float Sin2Phi(const Ubpa::normalf & w) { return SinPhi(w) * SinPhi(w); }

			inline float CosDPhi(const Ubpa::normalf & lhs, const Ubpa::normalf & rhs) {
				const float cosDPhi = (lhs[0]*rhs[0] + lhs[1]*rhs[1]) / ((lhs[0]*lhs[0] + lhs[1]*lhs[1])*(rhs[0]*rhs[0] + rhs[1]*rhs[1]));
				return Basic::Math::Clamp(cosDPhi, -1, 1);
			}

			inline Ubpa::normalf SphericalDirection(float sinTheta, float cosTheta, float phi) {
				return Ubpa::normalf(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
			}

			inline bool IsVisible(const Ubpa::normalf & w, const Ubpa::normalf & wh) { return CosTheta(w) * w.dot(wh) > 0; }

			inline bool IsEntering(const Ubpa::normalf & wo, const Ubpa::normalf & wh) { return wo.dot(wh) > 0.f; }

			inline bool IsEntering(const Ubpa::normalf & wo) { return CosTheta(wo) > 0.f; }

			inline bool IsSameSide(const Ubpa::normalf & wo, const Ubpa::normalf & wi) { return CosTheta(wo) * CosTheta(wi) > 0.f; }

			inline void SetOurward(Ubpa::normalf & wh) { wh *= Basic::Math::sgn(CosTheta(wh)); }

			inline const Ubpa::normalf Reflect(const Ubpa::normalf & w) { return Ubpa::normalf(-w[0], -w[1], w[2]); }

			inline bool Refract(const Ubpa::normalf & wo, Ubpa::normalf & wi, float ior) {
				float inv = IsEntering(wo) ? 1.0f / ior : ior;

				float discriminant = 1 - (1 - CosTheta(wo) * CosTheta(wo)) * inv * inv;
				if (discriminant <= 0)
					return false;

				wi[0] = -wo[0] * inv;
				wi[1] = -wo[1] * inv;
				wi[2] = -Basic::Math::sgn(wo[2]) * sqrt(discriminant);
				wi.normalize_self();

				return true;
			}

			inline bool IsOnSurf(const Ubpa::normalf & w) {
				return Basic::Math::is_all_zero(CosTheta(w));
			}
		}
	}
}

#endif // !_CPPUTIL_ENGINE_MATERIAL_SURF_COORD_H_
