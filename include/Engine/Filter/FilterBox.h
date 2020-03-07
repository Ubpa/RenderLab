#pragma once

#include "ImgFilter.h"

namespace Ubpa {
	class FilterBox : public ImgFilter {
	public:
		FilterBox(const vecf2& radius) : ImgFilter(radius) { }

	protected:
		virtual ~FilterBox() = default;

	public:
		const Ptr<FilterBox> New(const vecf2& radius) {
			return Ubpa::New<FilterBox>(radius);
		}

	public:
		virtual float Evaluate(const pointf2& p) const override {
			return 1;
		}
	};
}
