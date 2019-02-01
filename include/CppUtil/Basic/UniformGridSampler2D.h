#ifndef _UNIFORM_GRID_SAMPLER_2D_H_
#define _UNIFORM_GRID_SAMPLER_2D_H_

#include <CppUtil/Basic/Sampler2D.h>

namespace CppUtil {
	namespace Basic {
		class UniformGridSampler2D : public Sampler2D {
		public:
			glm::vec2 GetSample() const;
		};
	}
}

#endif//!_UNIFORM_GRID_SAMPLER_2D_H_
