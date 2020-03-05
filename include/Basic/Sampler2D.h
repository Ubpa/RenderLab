#pragma once

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
