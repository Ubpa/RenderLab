#pragma once

#include "Sampler3D.h"

namespace Ubpa {
	class CosHsSampler3D : public Sampler3D {
	public:
		virtual const valf3 GetSample();

		// Also returns the probability density at the sample point for use in importance sampling.
		virtual const valf3 GetSample(float& pd);
	};
}
