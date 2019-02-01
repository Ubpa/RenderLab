#ifndef _SAMPLER_2D_H_
#define _SAMPLER_2D_H_

#include <glm/vec2.hpp>

namespace CppUtil {
	namespace Basic {
		class Sampler2D {
		public:
			virtual glm::vec2 GetSample() const = 0;
			virtual ~Sampler2D() {}
		};
	}
}

#endif//!_SAMPLER_2D_H_
