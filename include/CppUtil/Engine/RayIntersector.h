#ifndef _RAY_INTERSECTOR_H_
#define _RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>
#include <CppUtil/Engine/BVHNode.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {
		class Ray;
		class Triangle;

		class RayIntersector : public Intersector {
			ELEVISITOR_SETUP(RayIntersector)
		public:
			struct Rst : public Intersector::Rst {
				Rst(bool isIntersect = false)
					: Intersector::Rst(isIntersect) { }

				glm::vec3 n;
			};

		public:
			RayIntersector();

		public:
			virtual const Rst & GetRst() const { return rst; }
			void SetRay(Basic::Ptr<Ray> ray) { this->ray = ray; }

			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			bool Intersect(const BBox & bbox);
			bool Intersect(const BBox & bbox, float & t0, float & t1);
			void Intersect(BVHNode<Triangle>::Ptr bvhNode);
			// 如果相交，则会修改 ray.tMax
			void Intersect(Basic::Ptr<Triangle> triangle);

		private:
			Basic::Ptr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_RAY_INTERSECTOR_H_