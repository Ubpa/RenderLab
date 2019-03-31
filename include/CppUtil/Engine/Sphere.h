#ifndef _ENGINE_PRIMITIVE_SPHERE_H_
#define _ENGINE_PRIMITIVE_SPHERE_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Basic/BBox.h>

namespace CppUtil {
	namespace Engine {
		class Sphere : public Primitive {
			ELE_SETUP(Sphere)
		public:
			const Basic::BBoxf GetBBox() const {
				return Basic::BBoxf(Basic::Pointf(-1.f), Basic::Pointf(1.f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_SPHERE_H_
