#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_MITCHELL_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_MITCHELL_H_

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterMitchell : public Filter {
		public:
			FilterMitchell(const Ubpa::vecf2 & radius, float B, float C) : Filter(radius), B(B), C(C) { }

		protected:
			virtual ~FilterMitchell() = default;

		public:
			static const Basic::Ptr<FilterMitchell> New(const Ubpa::vecf2 & radius, float B, float C) {
				return Basic::New<FilterMitchell>(radius, B, C);
			}

		public:
			virtual float Evaluate(const Ubpa::pointf2 & p) const override {
				return Mitchell1D(p[0] * invRadius[0]) * Mitchell1D(p[1] * invRadius[1]);
			}

		private:
			float Mitchell1D(float x) const;

		private:
			const float B;
			const float C;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_MITCHELL_H_
