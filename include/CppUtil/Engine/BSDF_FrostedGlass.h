#ifndef _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
#define _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class BSDF_FrostedGlass : public Engine::BSDF {
			ELE_SETUP(BSDF_FrostedGlass)
		public:
			BSDF_FrostedGlass(const glm::vec3 & colorFactor = glm::vec3(1), float roughnessFactor = 1.f, float ior = 1.5f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), ior(ior),
				colorTexture(nullptr),
				roughnessTexture(nullptr),
				aoTexture(nullptr),
				normalTexture(nullptr) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord) ;

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// PD is probability density
			// return color
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, const glm::vec2 & texcoord, glm::vec3 & wi, float & PD);

			virtual bool IsDelta() const { return false; }

			virtual void ChangeNormal(const glm::vec2 & texcoord, const glm::vec3 & tangent, glm::vec3 & normal) const;

		private:
			static float GGX_D(const glm::vec3 & h, float alpha);
			static float GGX_G1(const glm::vec3 & v, const glm::vec3 & h, float alpha);
			static float GGX_G(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec3 & h, float alpha);
			static float Fr(const glm::vec3 & v, const glm::vec3 & h, float ior);

		private:
			glm::vec3 GetColor(const glm::vec2 & texcoord) const;
			float GetRoughness(const glm::vec2 & texcoord) const;
			float GetAO(const glm::vec2 & texcoord) const;

		public:
			glm::vec3 colorFactor;
			Basic::Ptr<Basic::Image> colorTexture;

			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture;
			Basic::Ptr<Basic::Image> normalTexture;

			float ior;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
