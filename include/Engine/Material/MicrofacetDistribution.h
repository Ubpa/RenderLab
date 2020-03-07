#pragma once

#include <UGM/normal.h>
#include <Engine/Material/SurfCoord.h>

namespace Ubpa {
	class MicrofacetDistribution {
	public:
		// 法线分布函数
		virtual float D(const normalf& wh) const = 0;

		virtual const normalf Sample_wh() const = 0;

		virtual float G1(const normalf& w) const {
			return 1.f / (1.f + Lambda(w));
		}

		// 几何函数
		virtual float G(const normalf& wo, const normalf& wi, const normalf& wh) const {
			if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
				return 0.f;

			return 1.f / (1.f + Lambda(wo) + Lambda(wi));
		}

		// 几何函数 Smith
		virtual float G_Smith(const normalf& wo, const normalf& wi, const normalf& wh) const {
			if (!SurfCoord::IsVisible(wo, wh) || !SurfCoord::IsVisible(wi, wh))
				return 0.f;

			return G1(wo) * G1(wi);
		}

		// 这里的概率是关于 wh 的
		// path tracing 中需要的概率应是关于 wi 的
		float PDF(const normalf& wh) const {
			return D(wh) * abs(wh[2]);
		}

	protected:
		virtual float Lambda(const normalf& w) const = 0;
	};
}
