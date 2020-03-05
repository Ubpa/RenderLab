#pragma once

#include <Basic/Sampler3D.h>

namespace CppUtil {
	namespace Basic {
		class CosineWeightedHemisphereSampler3D : public Sampler3D {
		public:
			virtual const Ubpa::valf3 GetSample();

			// Also returns the probability density at the sample point for use in importance sampling.
			virtual const Ubpa::valf3 GetSample(float & pd);
		};
	}
}
