#ifndef _ENGINE_PRIMITIVE_PLANE_H_
#define _ENGINE_PRIMITIVE_PLANE_H_

#include <CppUtil/Engine/Primitive.h>

namespace CppUtil {
	namespace Engine {
		class Plane final : public Primitive {
		public:
			Plane() = default;
		public:
			static const Basic::Ptr<Plane> New() { return Basic::New<Plane>(); }

		protected:
			virtual ~Plane() = default;

		public:
			// primitive 局部坐标系内的 bbox
			virtual const BBoxf GetBBox() const override {
				return BBoxf(Point3(-0.5f, -0.001f, -0.5f), Point3(0.5f, 0.001f, 0.5f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PLANE_H_
