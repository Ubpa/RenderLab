#ifndef _ENGINE_PRIMITIVE_SPHERE_H_
#define _ENGINE_PRIMITIVE_SPHERE_H_

#include <CppUtil/Engine/Primitive.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Sphere : public Primitive {
			ELE_SETUP(Sphere)
		public:
			Sphere(const glm::vec3 & center = glm::vec3(0), float r = 1.0f)
				: center(center), r(r) { }

			const glm::vec3 & GetCenter() const { return center; }
			float GetR() const { return r; }
			void SetCenter(const glm::vec3 & center) { this->center = center; }
			void SetR(float r) { this->r = r; }
		private:
			glm::vec3 center;
			float r;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_SPHERE_H_
