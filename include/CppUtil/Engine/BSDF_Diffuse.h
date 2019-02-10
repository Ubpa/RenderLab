#ifndef _ENGINE_MATERIAL_BSDF_DIFFUSE_H_
#define _ENGINE_MATERIAL_BSDF_DIFFUSE_H_

#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Diffuse : public BSDF {
			ELE_SETUP(BSDF_Diffuse)
		public:
			BSDF_Diffuse(const glm::vec3 & albedo = glm::vec3(1)) :albedo(albedo) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi);

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi);

			// PD is probability density
			// return albedo
			// @arg0 in
			// @arg1 out
			// @arg2 out
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, glm::vec3 & wi, float & pd);

			virtual glm::vec3 GetEmission() const { return glm::vec3(0); }

			virtual bool IsDelta() const { return false; }

			glm::vec3 GetAlbedo()const { return albedo; }

		private:
			glm::vec3 albedo;
			Basic::CosineWeightedHemisphereSampler3D sampler;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_DIFFUSE_H_