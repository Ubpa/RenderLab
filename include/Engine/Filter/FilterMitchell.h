#pragma once

#include "ImgFilter.h"

namespace Ubpa {
	class FilterMitchell : public ImgFilter {
	public:
		FilterMitchell(const vecf2& radius, float B, float C) : ImgFilter(radius), B(B), C(C) { }

	protected:
		virtual ~FilterMitchell() = default;

	public:
		static const Ptr<FilterMitchell> New(const vecf2& radius, float B, float C) {
			return Ubpa::New<FilterMitchell>(radius, B, C);
		}

	public:
		virtual float Evaluate(const pointf2& p) const override {
			return Mitchell1D(p[0] * invRadius[0]) * Mitchell1D(p[1] * invRadius[1]);
		}

	private:
		float Mitchell1D(float x) const;

	private:
		const float B;
		const float C;
	};
}
