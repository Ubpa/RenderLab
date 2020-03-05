#pragma once

#include <Basic/Sampler2D.h>

namespace CppUtil {
	namespace Basic {
		class UniformGridSampler2D : public Sampler2D {
		public:
			virtual const Ubpa::valf2 GetSample() const;
		};
	}
}
