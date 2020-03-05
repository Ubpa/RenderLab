#pragma once

#include <Basic/Visitor.h>

namespace Ubpa {
	class SObj;

	class Sphere;
	class Plane;
	class Triangle;
	class TriMesh;
	class BVHAccel;
	class Disk;
	class Capsule;

	class Intersector : public Visitor {
	protected:
		Intersector() = default;
		virtual ~Intersector() = default;
	};
}
