#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <CppUtil/RTX/Hitable.h>

namespace RTX {
	class Triangle : public Hitable {
		HITABLE_SETUP(Triangle)
	public:
		Triangle(const Vertex & A, const Vertex & B, const Vertex & C, Material::CPtr material = NULL);

		virtual HitRst RayIn(CppUtil::Basic::Ptr<Ray> & ray) const;
		virtual const AABB GetBoundingBox() const { return box; }
		const Vertex & GetA() const { return A; }
		const Vertex & GetB() const { return B; }
		const Vertex & GetC() const { return C; }
	private:
		Vertex A, B, C;
		AABB box;
	};
}

#endif // !_TRIANGLE_H_
