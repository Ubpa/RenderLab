#pragma once

#include <UGM/val.h>

namespace Ubpa {
	class Sampler2D {
	public:
		virtual const valf2 GetSample() const = 0;
		virtual ~Sampler2D() {}
	};
}
