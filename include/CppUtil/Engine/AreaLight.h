#ifndef _ENGINE_AREA_LIGHT_H_
#define _ENGINE_AREA_LIGHT_H_

#include <CppUtil/Engine/LightBase.h>
#include <CppUtil/Basic/UniformGridSampler2D.h>

namespace CppUtil {
	namespace Engine {
		class AreaLight : public LightBase {
			ELE_SETUP(AreaLight)
		public:
			AreaLight(const glm::vec3 &color = glm::vec3(1), float width = 1, float height = 1, float intensity = 1)
				: color(color), width(width), height(height), intensity(intensity) { }

			virtual glm::vec3 Sample_L(const glm::vec3 & p, glm::vec3 & wi, float & distToLight, float & pd) const;
			virtual float PDF(const glm::vec3& p, const glm::vec3& wi) const;
			virtual bool IsDelta() const { return false; }

		public:
			float width;
			float height;
			glm::vec3 color;
			float intensity;

		private:
			Basic::UniformGridSampler2D sampler;
		};
	}
}

#endif//!_ENGINE_AREA_LIGHT_H_
