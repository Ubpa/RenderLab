#pragma once

#include <UGM/normal.h>
#include <Basic/Math.h>

namespace Ubpa {
	namespace SurfCoord {
		inline float CosTheta(const normalf& w) { return w[2]; }
		inline float Cos2Theta(const normalf& w) { return w[2] * w[2]; }
		inline float AbsCosTheta(const normalf& w) { return std::abs(w[2]); }
		inline float Sin2Theta(const normalf& w) { return 1.f - Cos2Theta(w); }
		inline float SinTheta(const normalf& w) { return std::sqrt(Sin2Theta(w)); }
		inline float TanTheta(const normalf& w) { return SinTheta(w) / CosTheta(w); }
		inline float Tan2Theta(const normalf& w) { return 1 / Cos2Theta(w) - 1; }
		inline float CosPhi(const normalf& w) {
			float sinTheta = SinTheta(w);
			return sinTheta == 0 ? 1 : Math::Clamp(w[0] / sinTheta, -1, 1);
		}

		inline float SinPhi(const normalf& w) {
			float sinTheta = SinTheta(w);
			return sinTheta == 0 ? 0 : Math::Clamp(w[1] / sinTheta, -1, 1);
		}

		inline float Cos2Phi(const normalf& w) { return CosPhi(w) * CosPhi(w); }

		inline float Sin2Phi(const normalf& w) { return SinPhi(w) * SinPhi(w); }

		inline float CosDPhi(const normalf& lhs, const normalf& rhs) {
			const float cosDPhi = (lhs[0] * rhs[0] + lhs[1] * rhs[1]) / ((lhs[0] * lhs[0] + lhs[1] * lhs[1]) * (rhs[0] * rhs[0] + rhs[1] * rhs[1]));
			return Math::Clamp(cosDPhi, -1, 1);
		}

		inline normalf SphericalDirection(float sinTheta, float cosTheta, float phi) {
			return normalf(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
		}

		inline bool IsVisible(const normalf& w, const normalf& wh) { return CosTheta(w) * w.dot(wh) > 0; }

		inline bool IsEntering(const normalf& wo, const normalf& wh) { return wo.dot(wh) > 0.f; }

		inline bool IsEntering(const normalf& wo) { return CosTheta(wo) > 0.f; }

		inline bool IsSameSide(const normalf& wo, const normalf& wi) { return CosTheta(wo) * CosTheta(wi) > 0.f; }

		inline void SetOurward(normalf& wh) { wh *= Math::sgn(CosTheta(wh)); }

		inline const normalf Reflect(const normalf& w) { return normalf(-w[0], -w[1], w[2]); }

		inline bool Refract(const normalf& wo, normalf& wi, float ior) {
			float inv = IsEntering(wo) ? 1.0f / ior : ior;

			float discriminant = 1 - (1 - CosTheta(wo) * CosTheta(wo)) * inv * inv;
			if (discriminant <= 0)
				return false;

			wi[0] = -wo[0] * inv;
			wi[1] = -wo[1] * inv;
			wi[2] = -Math::sgn(wo[2]) * sqrt(discriminant);
			wi.normalize_self();

			return true;
		}

		inline bool IsOnSurf(const normalf& w) {
			return Math::is_all_zero(CosTheta(w));
		}
	}
}
