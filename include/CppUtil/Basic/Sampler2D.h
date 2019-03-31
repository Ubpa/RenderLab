#ifndef _BASIC_SAMPLER_SAMPLER_2D_H_
#define _BASIC_SAMPLER_SAMPLER_2D_H_

#include <CppUtil/Basic/Point2.h>

namespace CppUtil {
	namespace Basic {
		class Sampler2D {
		public:
			virtual const Point2f GetSample() const = 0;
			virtual ~Sampler2D() {}
		};
	}
}

#endif//!_BASIC_SAMPLER_SAMPLER_2D_H_
