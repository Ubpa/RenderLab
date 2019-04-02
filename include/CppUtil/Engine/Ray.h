#ifndef _ENGINE_PRIMITIVE_RAY_H_
#define _ENGINE_PRIMITIVE_RAY_H_

#include <CppUtil/Basic/Ray.h>

namespace CppUtil {
	namespace Engine {
		class Ray : public Basic::Ray {
		public:
			Ray(const Point3 & origin = Point3(0), const Point3 & dir = Point3(1), float tMin = 0.001f, float tMax = FLT_MAX)
				: Basic::Ray(origin, dir), tMin(tMin), tMax(tMax) { }

		public:
			void Init(const Point3 & origin, const Vec3 & dir) {
				o = origin;
				d = dir;
				tMin = 0.001f;
				tMax = FLT_MAX;
			}

			const Point3 StartPos() const { return (*this)(tMin); }
			const Point3 EndPos() const { return (*this)(tMax); }

			const Val3f InvDir() const { return Val3f(1.f / d.x, 1.f / d.y, 1.f / d.z); }
		
		public:
			float tMin;
			float tMax;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_RAY_H_