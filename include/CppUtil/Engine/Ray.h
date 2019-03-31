#ifndef _ENGINE_PRIMITIVE_RAY_H_
#define _ENGINE_PRIMITIVE_RAY_H_

#include <CppUtil/Basic/Ray.h>

namespace CppUtil {
	namespace Engine {
		class Ray : public Basic::Ray {
		public:
			Ray(const Basic::Pointf & origin = Basic::Pointf(0), const Basic::Pointf & dir = Basic::Pointf(1), float tMin = 0.001f, float tMax = FLT_MAX)
				: Basic::Ray(origin, dir), tMin(tMin), tMax(tMax) { }

		public:
			void Init(const Basic::Pointf & origin, const Basic::Vectorf & dir) {
				o = origin;
				d = dir;
				tMin = 0.001f;
				tMax = FLT_MAX;
			}

			const Basic::Pointf StartPos() const { return (*this)(tMin); }
			const Basic::Pointf EndPos() const { return (*this)(tMax); }

			const Basic::Val3f InvDir() const { return Basic::Val3f(1.f / d.x, 1.f / d.y, 1.f / d.z); }
		
		public:
			float tMin;
			float tMax;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_RAY_H_