#ifndef _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
#define _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {

		// Disney
		class BSDF_MetalWorkflow : public BSDF {
			ELE_SETUP(BSDF_MetalWorkflow)
		public:
			BSDF_MetalWorkflow(const glm::vec3 & albedo, float metallic, float roughness)
				: albedo(albedo), metallic(metallic), roughness(roughness) { }

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

		private:
			// Microfacet Specular BRDF
			glm::vec3 MS_BRDF(const glm::vec3 & wo, const glm::vec3 & wi);
			// Normal Distribution Function
			float NDF(const glm::vec3 & h);
			// Fresnel
			glm::vec3 Fr(const glm::vec3 & wi, glm::vec3 & h);
			// Geometric Attenuation
			float G(const glm::vec3 & wo, const glm::vec3 & wi);

		public:
			glm::vec3 albedo;
			// 0--1
			float metallic;
			// 0--1
			float roughness;
		private:
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
