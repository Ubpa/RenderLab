#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_

#include <CppUtil/Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterTriangle : public Filter {
		public:
			FilterTriangle(const Vec2 & radius) : Filter(radius) { }

		protected:
			virtual ~FilterTriangle() = default;

		public:
			const Basic::Ptr<FilterTriangle> New(const Vec2 & radius) {
				return Basic::New<FilterTriangle>(radius);
			}

		public:
			virtual float Evaluate(const Point2f & p) const override {
				const auto delta = (radius - Vec2(p.Abs())).MaxWith({ 0,0 });
				return delta.x * delta.y;
			}
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_TRIANGLE_H_
