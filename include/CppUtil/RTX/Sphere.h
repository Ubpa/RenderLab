#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <CppUtil/RTX/Hitable.h>

namespace RTX {
	class Sphere : public Hitable {
		HITABLE_SETUP(Sphere)
	public:
		Sphere(const glm::vec3 & center = glm::vec3(0.0f), float radius = 1.0f, Material::CPtr material = NULL);

		virtual HitRst RayIn(CppUtil::Basic::Ptr<Ray> & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }

		const glm::vec3 & GetCenter() const { return center; }
		float GetRadius() const { return radius; }
	protected:

		glm::vec3 center;
		float radius;
		AABB box;
	};
}

#endif // !_SPHERE_H_
