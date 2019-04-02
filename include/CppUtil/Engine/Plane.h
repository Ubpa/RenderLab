#ifndef _ENGINE_PRIMITIVE_PLANE_H_
#define _ENGINE_PRIMITIVE_PLANE_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Basic/BBox.h>

namespace CppUtil {
	namespace Engine {
		class Plane : public Primitive {
			ELE_SETUP(Plane)

		public:
			// primitive 局部坐标系内的 bbox
			const BBoxf GetBBox() const {
				return BBoxf(Point3(-0.5f, -0.000001f, -0.5f), Point3(0.5f, 0.000001f, 0.5f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PLANE_H_
