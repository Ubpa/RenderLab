#ifndef _RAY_INTERSECTOR_H_
#define _RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class RayIntersector : public Intersector {
			ELEVISITOR_SETUP(RayIntersector)
		public:
			struct Rst : public Intersector::Rst {
				Rst(bool isIntersect = false, float t = 0)
					: Intersector::Rst(isIntersect), t(t) { }

				glm::vec3 n;
				float t;
			};

		public:
			RayIntersector();

		public:
			virtual const Rst & GetRst() const { return rst; }
			void SetRay(Basic::CPtr<Ray> ray) { this->ray = ray; }

			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);

		private:
			Basic::CPtr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_RAY_INTERSECTOR_H_