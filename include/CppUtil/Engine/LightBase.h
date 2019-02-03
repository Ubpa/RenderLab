#ifndef _ENGINE_LIGHT_BASE_H_
#define _ENGINE_LIGHT_BASE_H_

#include <CppUtil/Basic/Element.h>

#include <glm/glm.hpp>

namespace CppUtil {
	namespace Engine {
		class LightBase : public Basic::Element {
			ELE_SETUP(LightBase)
		public:
			virtual glm::vec3 Sample_L(const glm::vec3& p, glm::vec3 & wi, float & distToLight, float & pd) const = 0;
			virtual float PDF(const glm::vec3& p, const glm::vec3& wi) const = 0;
			virtual bool IsDelta() const = 0;
		};
	}
}

#endif//!_ENGINE_LIGHT_BASE_H_
