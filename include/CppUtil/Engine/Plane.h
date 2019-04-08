#ifndef _ENGINE_PRIMITIVE_PLANE_H_
#define _ENGINE_PRIMITIVE_PLANE_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Basic/UGM/BBox.h>

namespace CppUtil {
	namespace Engine {
		class Plane final : public Primitive<Plane> {
		public:
			Plane() = default;
		public:
			static const Basic::Ptr<Plane> New() { return Basic::New<Plane>(); }

		protected:
			virtual ~Plane() = default;

		public:
			// primitive 局部坐标系内的 bbox
			const BBoxf GetBBox() const {
				return BBoxf(Point3(-0.5f, -0.000001f, -0.5f), Point3(0.5f, 0.000001f, 0.5f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PLANE_H_
