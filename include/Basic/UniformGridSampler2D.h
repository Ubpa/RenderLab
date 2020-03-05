#pragma once

#include <Basic/Sampler2D.h>

namespace Ubpa {
	class UniformGridSampler2D : public Sampler2D {
	public:
		virtual const valf2 GetSample() const;
	};
}
