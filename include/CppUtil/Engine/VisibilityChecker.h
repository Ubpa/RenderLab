#ifndef _ENGINE_VISIBILITY_CHECKER_H_
#define _ENGINE_VISIBILITY_CHECKER_H_

#include <CppUtil/Engine/Intersector.h>
#include <CppUtil/Engine/BVHNode.h>

namespace CppUtil {
	namespace Engine {
		class Ray;
		class Triangle;

		class VisibilityChecker : public Intersector {
			ELEVISITOR_SETUP(VisibilityChecker)
		public:
			struct Rst : public Intersector::Rst {
				friend class VisibilityChecker;

				Rst(bool isIntersect = false)
					: Intersector::Rst(isIntersect) { }

				bool IsVisible() const { return isIntersect; }
			};

		public:
			VisibilityChecker(Basic::Ptr<Ray> ray, float tMax);

			virtual const Rst & GetRst() const { return rst; }

			void Visit(Basic::Ptr<SObj> sobj);
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			bool Intersect(const BBox & bbox);
			bool Intersect(const BBox & bbox, float & t0, float & t1);
			void Intersect(BVHNode<Triangle>::Ptr bvhNode);
			// 如果相交，则会设置 rst，修改 ray.tMax
			void Intersect(Basic::Ptr<Triangle> triangle);

		private:
			Basic::Ptr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_VISIBILITY_CHECKER_H_
