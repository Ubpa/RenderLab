#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterSinc : public Filter {
		public:
			FilterSinc(const Vec2 & radius, float tau) : Filter(radius), tau(tau) { }

		protected:
			virtual ~FilterSinc() = default;

		public:
			const Basic::Ptr<FilterSinc> New(const Vec2 & radius, float tau) {
				return Basic::New<FilterSinc>(radius, tau);
			}

		public:
			virtual float Evaluate(const Point2f & p) const override {
				return WindowSinc(p.x, radius.x) * WindowSinc(p.y, radius.y);
			}

		private:
			float WindowSinc(float x, float radius) const;

		private:
			const float tau;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_
