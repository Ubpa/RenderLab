#ifndef _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
#define _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_

#include <CppUtil/Engine/Intersector.h>

namespace CppUtil {
	namespace Basic {
		class Element;
	}

	namespace Engine {
		class SObj;
		class Ray;

		class BBox;
		class Sphere;
		class Plane;
		class Triangle;
		class BVHAccel;
		template <typename T, typename HolderT>
		class BVHNode;

		class VisibilityChecker : public Intersector {
			ELEVISITOR_SETUP(VisibilityChecker)
		public:
			struct Rst : public Intersector::Rst {
				friend class VisibilityChecker;

				Rst(bool isIntersect = false)
					: Intersector::Rst(isIntersect) { }

				bool IsIntersect() const { return isIntersect; }
			};

		public:
			VisibilityChecker(Basic::Ptr<Ray> ray, float tMax);

			virtual const Rst & GetRst() const { return rst; }

		private:
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<BVHAccel> bvhAccel);
			void Visit(Basic::Ptr<BVHNode<Basic::Element, BVHAccel>> bvhNode);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<Triangle> triangle);

		private:
			bool Intersect(const BBox & bbox);
			bool Intersect(const BBox & bbox, float & t0, float & t1);

		private:
			Basic::Ptr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
