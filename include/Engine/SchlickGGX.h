#ifndef _CPPUTIL_ENGINE_MATERIAL_SCHLICK_GGX_H_
#define _CPPUTIL_ENGINE_MATERIAL_SCHLICK_GGX_H_

#include <Engine/GGX.h>

namespace CppUtil {
	namespace Engine {
		// GGX 和 Schlick-Beckmann 近似的结合体
		class SchlickGGX : public GGX {
		public:
			virtual float G(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::normalf & wh) const override {
				// Smith's method
				if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
					return 0.f;

				return G1(wo) * G1(wi);
			}

			virtual float G1(const Ubpa::normalf & w) const override {
				auto k = alpha / 2;
				auto NoW = std::max(0.f, SurfCoord::CosTheta(w));
				return NoW / (NoW * (1 - k) + k);
			}
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MATERIAL_SCHLICK_GGX_H_
