#ifndef _RAY_INTERSECTOR_H_
#define _RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class RayIntersector : public Intersector {
			HEAP_OBJ_SETUP_SELF_DEL(RayIntersector)
		public:
			struct Rst : public Intersector::Rst {
				Rst(bool isIntersect = false, float t = 0)
					: Intersector::Rst(isIntersect), t(t) { }

				float t;
			};

		public:
			RayIntersector(Basic::CPtr<Ray> ray);

		protected:
			~RayIntersector();

		public:

			Basic::CPtr<Ray> GetRay() const { return ray; }

			virtual const Rst * GetRst() const { return &rst; }

			virtual void Intersect(Basic::CPtr<Sphere> sphere);

		private:
			Basic::CPtr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_RAY_INTERSECTOR_H_