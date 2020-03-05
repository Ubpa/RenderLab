#pragma once

#include <Basic/Visitor.h>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Sphere;
		class Plane;
		class Triangle;
		class TriMesh;
		class BVHAccel;
		class Disk;
		class Capsule;

		class Intersector : public Basic::Visitor {
		protected:
			Intersector() = default;
			virtual ~Intersector() = default;
		};
	}
}
