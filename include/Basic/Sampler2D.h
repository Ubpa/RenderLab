#ifndef _BASIC_SAMPLER_SAMPLER_2D_H_
#define _BASIC_SAMPLER_SAMPLER_2D_H_

#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class Sampler2D {
		public:
			virtual const Ubpa::valf2 GetSample() const = 0;
			virtual ~Sampler2D() {}
		};
	}
}

#endif//!_BASIC_SAMPLER_SAMPLER_2D_H_
