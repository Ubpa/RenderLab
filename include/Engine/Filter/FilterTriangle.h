#pragma once

#include "ImgFilter.h"

namespace Ubpa {
	class FilterTriangle : public ImgFilter {
	public:
		FilterTriangle(const vecf2& radius) : ImgFilter(radius) { }

	protected:
		virtual ~FilterTriangle() = default;

	public:
		const Ptr<FilterTriangle> New(const vecf2& radius) {
			return Ubpa::New<FilterTriangle>(radius);
		}

	public:
		virtual float Evaluate(const pointf2& p) const override {
			const auto delta = (radius - vecf2(p.Abs())).MaxWith({ 0,0 });
			return delta[0] * delta[1];
		}
	};
}
