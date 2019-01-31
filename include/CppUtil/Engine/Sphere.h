#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <CppUtil/Engine/Primitive.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Sphere : public Primitive {
			ELE_SETUP(Sphere)
		public:
			Sphere(const glm::vec3 & center, float r)
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

#endif//!_SPHERE_H_