#ifndef _CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_
#define _CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterSinc : public Filter {
		public:
			FilterSinc(const Ubpa::vecf2 & radius, float tau) : Filter(radius), tau(tau) { }

		protected:
			virtual ~FilterSinc() = default;

		public:
			const Basic::Ptr<FilterSinc> New(const Ubpa::vecf2 & radius, float tau) {
				return Basic::New<FilterSinc>(radius, tau);
			}

		public:
			virtual float Evaluate(const Ubpa::pointf2 & p) const override {
				return WindowSinc(p[0], radius[0]) * WindowSinc(p[1], radius[1]);
			}

		private:
			float WindowSinc(float x, float radius) const;

		private:
			const float tau;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_FILTER_FILTER_SINC_H_
