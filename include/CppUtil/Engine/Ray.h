#ifndef _PRIMITIVE_RAY_H_
#define _PRIMITIVE_RAY_H_

#include <CppUtil/Engine/Primitive.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace CppUtil {
	namespace Engine {
		class Ray : public Primitive {
			ELE_SETUP(Ray)
		public:
			Ray(const glm::vec3 & origin, const glm::vec3 dir, float tMin = 0.001, float tMax = FLT_MAX)
				: origin(origin), dir(dir), tMin(tMin), tMax(tMax) { }

			const glm::vec3 GetOrigin() const { return origin; }
			const glm::vec3 GetDir() const { return dir; }
			float GetTMin() const { return tMin; }
			float GetTMax() const { return tMax; }

			void Transform(const glm::mat4 & mat);

			const glm::vec3 At(float t) const { return origin + t * dir; }
		private:
			glm::vec3 origin;
			glm::vec3 dir;
			float tMin;
			float tMax;
		};
	}
}

#endif//!_PRIMITIVE_RAY_H_