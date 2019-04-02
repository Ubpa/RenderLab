#ifndef _ENGINE_PRIMITIVE_SPHERE_H_
#define _ENGINE_PRIMITIVE_SPHERE_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Basic/BBox.h>

namespace CppUtil {
	namespace Engine {
		class Sphere : public Primitive {
			ELE_SETUP(Sphere)
		public:
			const BBoxf GetBBox() const {
				return BBoxf(Point3(-1.f), Point3(1.f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_SPHERE_H_
