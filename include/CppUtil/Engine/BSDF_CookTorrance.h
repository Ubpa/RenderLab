#ifndef _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_
#define _ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_CookTorrance : public BSDF {
			ELE_SETUP(BSDF_CookTorrance)
		public:
			BSDF_CookTorrance(float ior, float m, glm::vec3 refletance=glm::vec3(1), glm::vec3 albedo = glm::vec3(1))
				: ior(ior), m(m), refletance(refletance) , albedo(albedo){ }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord);

			// PD is probability density
			// return refletance
			// @arg0 in
			// @arg1 out
			// @arg2 out
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, const glm::vec2 & texcoord, glm::vec3 & wi, float & pd);

			virtual bool IsDelta() const { return false; }

		private:
			float NDF(const glm::vec3 & h);
			float Fr(const glm::vec3 & wi, glm::vec3 & h);
			float G(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec3 & h);

		public:
			float ior;
			float m;
			glm::vec3 refletance;
			glm::vec3 albedo;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_COOK_TORRANCE_H_
