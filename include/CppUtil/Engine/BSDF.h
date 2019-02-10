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

			// PD is probability density
			// return albedo
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, glm::vec3 & wi, float & PD) = 0;

			virtual glm::vec3 GetEmission() const = 0;

			virtual bool IsDelta() const = 0;

		public:
			static glm::vec3 LocalReflect(const glm::vec3 & w);
			static bool LocalRefract(const glm::vec3& wo, glm::vec3& wi, float ior);
		};
	}
}

#endif//!_BSDF_H_