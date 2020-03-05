#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterTriangle : public Filter {
		public:
			FilterTriangle(const Ubpa::vecf2 & radius) : Filter(radius) { }

		protected:
			virtual ~FilterTriangle() = default;

		public:
			const Basic::Ptr<FilterTriangle> New(const Ubpa::vecf2 & radius) {
				return Basic::New<FilterTriangle>(radius);
			}

		public:
			virtual float Evaluate(const Ubpa::pointf2 & p) const override {
				const auto delta = (radius - Ubpa::vecf2(p.Abs())).MaxWith({ 0,0 });
				return delta[0] * delta[1];
			}
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_
