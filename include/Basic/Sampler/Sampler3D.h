#pragma once

#include <UGM/val.h>

namespace Ubpa {
	class Sampler3D {
	public:
		virtual const valf3 GetSample() = 0;
		virtual const valf3 GetSample(float& pd) = 0;
		virtual ~Sampler3D() {}
	};
}
