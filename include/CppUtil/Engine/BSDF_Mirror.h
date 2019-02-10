#ifndef _BSDF_MIRROR_H_
#define _BSDF_MIRROR_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Mirror : public BSDF {
			ELE_SETUP(BSDF_Mirror)
		public:
			BSDF_Mirror(const glm::vec3 & reflectance = glm::vec3(1)) : reflectance(reflectance) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi) { return glm::vec3(0); }

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi) { return 0; }

			// PD is probability density
			// return albedo
			// @arg0 in
			// @arg1 out
			// @arg2 out
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, glm::vec3 & wi, float & PD);

			virtual glm::vec3 GetEmission() const { return glm::vec3(0); }

			virtual bool IsDelta() const { return true; }

			glm::vec3 GetReflectance() const { return reflectance; }

		private:
			glm::vec3 reflectance;
		};
	}
}

#endif//!_BSDF_MIRROR_H_
