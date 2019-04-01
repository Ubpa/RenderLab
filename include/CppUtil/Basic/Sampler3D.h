#ifndef _BASIC_SAMPLER_SAMPLER_3D_H_
#define _BASIC_SAMPLER_SAMPLER_3D_H_

#include <CppUtil/Basic/Vector.h>

namespace CppUtil {
	namespace Basic {
		class Sampler3D {
		public:
			virtual const Vectorf GetSample() = 0;
			virtual const Vectorf GetSample(float & pd);
			virtual ~Sampler3D() {}
		};
	}
}

#endif//!_BASIC_SAMPLER_SAMPLER_3D_H_
