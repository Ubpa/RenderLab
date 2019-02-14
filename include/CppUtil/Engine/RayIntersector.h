#ifndef _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>
#include <glm/vec3.hpp>

namespace CppUtil {
	namespace Engine {

		// 寻找最近的交点
		class RayIntersector : public Intersector {
			ELEVISITOR_SETUP(RayIntersector)
		public:
			// isIntersect 用于判断与 Primitive 是否相交
			// closestSObj 用于记录最近的SObj
			// n 用于记录最近的相交处的法向
			struct Rst : public Intersector::Rst {
				friend class RayIntersector;

				Rst(bool isIntersect = false)
					: Intersector::Rst(isIntersect), closestSObj(nullptr), n(0) { }

				Basic::Ptr<SObj> closestSObj;
				glm::vec3 n;
			};

		public:
			RayIntersector(Basic::Ptr<Ray> ray);

		public:
			virtual const Rst & GetRst() const { return rst; }

			void Visit(Basic::Ptr<SObj> sobj);
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			bool Intersect(const BBox & bbox);
			bool Intersect(const BBox & bbox, float & t0, float & t1);
			template<typename T>
			void Intersect(typename BVHNode<T>::Ptr bvhNode);
			// 如果相交，则会设置 rst，修改 ray.tMax
			void Intersect(Basic::Ptr<Triangle> triangle);

		private:
			Basic::Ptr<Ray> ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_