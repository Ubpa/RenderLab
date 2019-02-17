#ifndef _ENGINE_MATERIAL_BSDF_EMISSION_H_
#define _ENGINE_MATERIAL_BSDF_EMISSION_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Emission : public BSDF {
			ELE_SETUP(BSDF_Emission)
		public:
			BSDF_Emission(const glm::vec3 & color = glm::vec3(1), float intensity = 1.0f)
				: color(color), intensity(intensity) { }

			virtual glm::vec3 F(const glm::vec3 & wo, const glm::vec3 & wi, const glm::vec2 & texcoord) { return glm::vec3(0); }

			// probability density function
			virtual float PDF(const glm::vec3 & wo, const glm::vec3 & wi) { return 0; }

			// PD is probability density
			// return albedo
			// @arg0 in
			// @arg1 out
			// @arg2 out
			virtual glm::vec3 Sample_f(const glm::vec3 & wo, const glm::vec2 & texcoord, glm::vec3 & wi, float & pd) { pd = 0; return glm::vec3(0); }

			virtual glm::vec3 GetEmission() const { return intensity * color; }

			virtual bool IsDelta() const { return false; }

		public:
			glm::vec3 color;
			float intensity;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_EMISSION_H_
