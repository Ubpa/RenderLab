#pragma once

#include <Engine/Material/GGX.h>

namespace Ubpa {
	// GGX 和 Schlick-Beckmann 近似的结合体
	class SchlickGGX : public GGX {
	public:
		virtual float G(const normalf& wo, const normalf& wi, const normalf& wh) const override {
			// Smith's method
			if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
				return 0.f;

			return G1(wo) * G1(wi);
		}

		virtual float G1(const normalf& w) const override {
			auto k = alpha / 2;
			auto NoW = std::max(0.f, SurfCoord::CosTheta(w));
			return NoW / (NoW * (1 - k) + k);
		}
	};
}
