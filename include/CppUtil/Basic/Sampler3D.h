#ifndef _BASIC_SAMPLER_SAMPLER_3D_H_
#define _BASIC_SAMPLER_SAMPLER_3D_H_

#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Basic {
		class Sampler3D {
		public:
			virtual glm::vec3 GetSample() const = 0;
			virtual ~Sampler3D() {}
		};
	}
}

#endif//!_BASIC_SAMPLER_SAMPLER_3D_H_
