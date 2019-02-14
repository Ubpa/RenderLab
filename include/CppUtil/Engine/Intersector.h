#ifndef _ENGINE_INTERSECTOR_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_INTERSECTOR_H_

#include <CppUtil/Basic/EleVisitor.h>

namespace CppUtil {
	namespace Engine {
		class SObj;
		class Sphere;
		class Plane;
		class TriMesh;

		class Ray;
		class Triangle;
		class BBox;
		template <typename T>
		class BVHNode;

		class Intersector : public Basic::EleVisitor {
			ELEVISITOR_SETUP(Intersector)
		public:
			struct Rst {
				Rst(bool isIntersect = false)
					: isIntersect(false) { }

			protected:
				bool isIntersect;
			};

		public:
			virtual const Rst & GetRst() const = 0;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_INTERSECTOR_H_