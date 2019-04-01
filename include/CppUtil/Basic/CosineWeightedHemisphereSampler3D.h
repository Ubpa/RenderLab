#ifndef _BASIC_SAMPLER_COSINE_WEIGHTED_HEMISPHERE_SAMPLER_3D_H_
#define _BASIC_SAMPLER_COSINE_WEIGHTED_HEMISPHERE_SAMPLER_3D_H_

#include <CppUtil/Basic/Sampler3D.h>

namespace CppUtil {
	namespace Basic {
		class CosineWeightedHemisphereSampler3D : public Sampler3D {
		public:
			virtual const Vectorf GetSample();

			// Also returns the probability density at the sample point for use in importance sampling.
			virtual const Vectorf GetSample(float & pd);
		};
	}
}

#endif//!_BASIC_SAMPLER_COSINE_WEIGHTED_HEMISPHERE_SAMPLER_3D_H_
