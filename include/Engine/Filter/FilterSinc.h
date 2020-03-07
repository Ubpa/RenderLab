#pragma once

#include "ImgFilter.h"

namespace Ubpa {
	class FilterSinc : public ImgFilter {
	public:
		FilterSinc(const vecf2& radius, float tau) : ImgFilter(radius), tau(tau) { }

	protected:
		virtual ~FilterSinc() = default;

	public:
		const Ptr<FilterSinc> New(const vecf2& radius, float tau) {
			return Ubpa::New<FilterSinc>(radius, tau);
		}

	public:
		virtual float Evaluate(const pointf2& p) const override {
			return WindowSinc(p[0], radius[0]) * WindowSinc(p[1], radius[1]);
		}

	private:
		float WindowSinc(float x, float radius) const;

	private:
		const float tau;
	};
}
