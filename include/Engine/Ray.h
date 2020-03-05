#ifndef _ENGINE_PRIMITIVE_RAY_H_
#define _ENGINE_PRIMITIVE_RAY_H_

#include <UGM/ray.h>

namespace CppUtil {
	namespace Engine {
		class Ray : public Ubpa::rayf3 {
		public:
			Ray(const Ubpa::pointf3 & origin = Ubpa::pointf3(0.f,0.f,0.f), const Ubpa::vecf3& dir = Ubpa::vecf3(1.f,1.f,1.f), float tMin = 0.001f, float tMax = FLT_MAX)
				: Ubpa::rayf3(origin, dir), tMin(tMin), tMax(tMax) { }

		public:
			const Ubpa::pointf3 StartPos() const { return (*this)(tMin); }
			const Ubpa::pointf3 EndPos() const { return (*this)(tMax); }

			const Ubpa::valf3 InvDir() const { return{ 1.f / d[0], 1.f / d[1], 1.f / d[2] }; }
		
		public:
			float tMin;
			float tMax;
		};

	}
	using ERay = Engine::Ray;
}

#endif//!_ENGINE_PRIMITIVE_RAY_H_