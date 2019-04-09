#ifndef _ENGINE_PRIMITIVE_SPHERE_H_
#define _ENGINE_PRIMITIVE_SPHERE_H_

#include <CppUtil/Engine/Primitive.h>
#include <CppUtil/Basic/UGM/BBox.h>

namespace CppUtil {
	namespace Engine {
		class Sphere final : public Primitive {
		public:
			Sphere() = default;
		public:
			static const Basic::Ptr<Sphere> New() { return Basic::New<Sphere>(); }

		protected:
			virtual ~Sphere() = default;

		public:
			const BBoxf GetBBox() const {
				return BBoxf(Point3(-1.f), Point3(1.f));
			}
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_SPHERE_H_
