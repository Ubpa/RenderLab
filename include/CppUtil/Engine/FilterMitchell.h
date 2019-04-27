#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_MITCHELL_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_MITCHELL_H_

#include <CppUtil/Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterMitchell : public Filter {
		public:
			FilterMitchell(const Val2 & radius, float B, float C) : Filter(radius), B(B), C(C) { }

		protected:
			virtual ~FilterMitchell() = default;

		public:
			static const Basic::Ptr<FilterMitchell> New(const Val2 & radius, float B, float C) {
				return Basic::New<FilterMitchell>(radius, B, C);
			}

		public:
			virtual float Evaluate(const Point2f & p) override {
				return Mitchell1D(p.x * invRadius.x) * Mitchell1D(p.y * invRadius.y);
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
