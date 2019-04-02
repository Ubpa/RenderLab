#ifndef _BASIC_SAMPLER_SAMPLER_2D_H_
#define _BASIC_SAMPLER_SAMPLER_2D_H_

#include <CppUtil/Basic/Val2.h>

namespace CppUtil {
	namespace Basic {
		class Sampler2D {
		public:
			virtual const Val2 GetSample() const = 0;
			virtual ~Sampler2D() {}
		};
	}
}

#endif//!_BASIC_SAMPLER_SAMPLER_2D_H_
