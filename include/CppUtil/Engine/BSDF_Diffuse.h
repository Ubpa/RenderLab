#ifndef _BSDF_DIFFUSE_H_
#define _BSDF_DIFFUSE_H_

#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Diffuse : public BSDF {
			ELE_SETUP(BSDF_Diffuse)
		public:
			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi) = 0;

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi) = 0;

			// pd is probability density
			// return albedo
			virtual glm::vec3 Sample(const glm::vec3 & wo, glm::vec3 & wi, float & pd) = 0;

			virtual glm::vec3 GetEmission() const { return glm::vec3(0); }

			virtual bool IsDelta() const { return false; }

		private:
			glm::vec3 albedo;
			Basic::CosineWeightedHemisphereSampler3D sampler;
		};
	}
}

#endif//!_BSDF_DIFFUSE_H_