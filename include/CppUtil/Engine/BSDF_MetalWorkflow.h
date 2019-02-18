#ifndef _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
#define _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {

		// Disney
		class BSDF_MetalWorkflow : public BSDF {
			ELE_SETUP(BSDF_MetalWorkflow)
		public:
			BSDF_MetalWorkflow(const glm::vec3 & albedoColor = glm::vec3(1.0f), float roughnessFactor = 1.0f, float metallicFactor = 1.0f)
				: albedoColor(albedoColor), roughnessFactor(roughnessFactor), metallicFactor(metallicFactor),
				albedoTexture(nullptr), metallicTexture(nullptr), aoTexture(nullptr) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// PD is probability density
			// return albedo
			// @arg0 in
			// @arg1 out
			// @arg2 out
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, const glm::vec2 & texcoord, glm::vec3 & wi, float & pd);

			virtual bool IsDelta() const { return false; }

			virtual void ChangeNormal(const glm::vec2 & texcoord, const glm::vec3 tangent, glm::vec3 & normal) const;

		public:
			void SetAlbedoTexture(Basic::Ptr<Basic::Image> albedoTexture) {
				this->albedoTexture = albedoTexture;
			}

			void SetMetallicTexture(Basic::Ptr<Basic::Image> metallicTexture) {
				this->metallicTexture = metallicTexture;
			}

			void SetRoughnessTexture(Basic::Ptr<Basic::Image> roughnessTexture) {
				this->roughnessTexture = roughnessTexture;
			}

			void SetAOTexture(Basic::Ptr<Basic::Image> aoTexture) {
				this->aoTexture = aoTexture;
			}

			void SetNormalTexture(Basic::Ptr<Basic::Image> normalTexture) {
				this->normalTexture = normalTexture;
			}

		private:
			// Microfacet Specular BRDF
			static glm::vec3 MS_BRDF(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec3 & albedo, float metallic, float roughness);
			// Normal Distribution Function
			static float NDF(const glm::vec3 & h, float roughness);
			// Fresnel
			static glm::vec3 Fr(const glm::vec3 & wi, const glm::vec3 & h, const glm::vec3 & albedo, float metallic);
			// Geometric Attenuation
			static float G(const glm::vec3 & wo, const glm::vec3 & wi, float roughness);

			const glm::vec3 GetAlbedo(const glm::vec2 & texcoord) const;
			float GetMetallic(const glm::vec2 & texcoord) const;
			float GetRoughness(const glm::vec2 & texcoord) const;
			float GetAO(const glm::vec2 & texcoord) const;

		public:
			glm::vec3 albedoColor;
			Basic::Ptr<Basic::Image> albedoTexture;

			// 0--1
			float metallicFactor;
			Basic::Ptr<Basic::Image> metallicTexture;

			// 0--1
			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture;

			Basic::Ptr<Basic::Image> normalTexture;
		private:
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
