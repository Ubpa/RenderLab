#ifndef _BSDF_H_
#define _BSDF_H_

#include <CppUtil/Engine/MaterialBase.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class BSDF : public MaterialBase {
			ELE_SETUP(BSDF)
		public:
			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi) = 0;

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi) = 0;

			// pd is probability density
			// return albedo
			virtual glm::vec3 Sample(const glm::vec3 & wo, glm::vec3 & wi, float & pd) = 0;

			virtual glm::vec3 GetEmission() const = 0;

			virtual bool IsDelta() const = 0;
		};
	}
}

#endif//!_BSDF_H_