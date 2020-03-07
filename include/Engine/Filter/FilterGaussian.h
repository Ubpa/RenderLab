#pragma once

#include "ImgFilter.h"

namespace Ubpa {
	class FilterGaussian : public ImgFilter {
	public:
		FilterGaussian(const vecf2& radius, float alpha)
			: ImgFilter(radius), alpha(alpha),
			expX(std::exp(-alpha * radius[0] * radius[0])),
			expY(std::exp(-alpha * radius[1] * radius[1])) { }

	protected:
		virtual ~FilterGaussian() = default;

	public:
		const Ptr<FilterGaussian> New(const vecf2& radius, float alpha) {
			return Ubpa::New<FilterGaussian>(radius, alpha);
		}

	public:
		virtual float Evaluate(const pointf2& p) const override {
			return Gaussian(p[0], expX) * Gaussian(p[1], expY);
		}

	private:
		float Gaussian(float d, float expv) const {
			return std::max(0.f, std::exp(-alpha * d * d) - expv);
		}

	private:
		const float alpha;
		const float expX;
		const float expY;
	};
}
