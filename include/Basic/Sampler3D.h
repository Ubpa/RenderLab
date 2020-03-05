#pragma once

#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class Sampler3D {
		public:
			virtual const Ubpa::valf3 GetSample() = 0;
			virtual const Ubpa::valf3 GetSample(float & pd) = 0;
			virtual ~Sampler3D() {}
		};
	}
}
