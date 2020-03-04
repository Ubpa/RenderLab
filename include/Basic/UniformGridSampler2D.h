#ifndef _BASIC_SAMPLER_UNIFORM_GRID_SAMPLER_2D_H_
#define _BASIC_SAMPLER_UNIFORM_GRID_SAMPLER_2D_H_

#include <Basic/Sampler2D.h>

namespace CppUtil {
	namespace Basic {
		class UniformGridSampler2D : public Sampler2D {
		public:
			virtual const Val2 GetSample() const;
		};
	}
}

#endif//!_BASIC_SAMPLER_UNIFORM_GRID_SAMPLER_2D_H_
