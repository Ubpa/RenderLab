#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_GAUSSIAN_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_GAUSSIAN_H_

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterGaussian : public Filter {
		public:
			FilterGaussian(const Vec2 & radius, float alpha)
				: Filter(radius), alpha(alpha),
				expX(std::exp(-alpha * radius.x*radius.x)),
				expY(std::exp(-alpha * radius.y*radius.y)) { }

		protected:
			virtual ~FilterGaussian() = default;

		public:
			const Basic::Ptr<FilterGaussian> New(const Vec2 & radius, float alpha) {
				return Basic::New<FilterGaussian>(radius, alpha);
			}

		public:
			virtual float Evaluate(const Point2f & p) const override {
				return Gaussian(p.x, expX) * Gaussian(p.y, expY);
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
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_GAUSSIAN_H_
