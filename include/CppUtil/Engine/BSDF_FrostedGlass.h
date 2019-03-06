#ifndef _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
#define _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_FrostedGlass : public Engine::BSDF {
			ELE_SETUP(BSDF_FrostedGlass)
		public:
			BSDF_FrostedGlass(const glm::vec3 & albedo = glm::vec3(1), float alpha = 0.2f, float ior = 1.5f)
				: albedo(albedo), alpha(alpha), ior(ior) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord) ;

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// PD is probability density
			// return albedo
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, const glm::vec2 & texcoord, glm::vec3 & wi, float & PD);

			virtual bool IsDelta() const { return false; }

		private:
			float D(const glm::vec3 & h);
			float G1(const glm::vec3 & v, const glm::vec3 & h);
			float G(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec3 & h);
			float Fr(const glm::vec3 & v, const glm::vec3 & h);

		public:
			glm::vec3 albedo;
			float alpha;
			float ior;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
