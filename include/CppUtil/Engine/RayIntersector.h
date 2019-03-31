#ifndef _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Point2.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/BBox.h>

#include <CppUtil/Engine/Ray.h>

namespace CppUtil {
	namespace Basic {
		class Element;
	}

	namespace Engine {
		class SObj;

		class Sphere;
		class Plane;
		class Triangle;
		class BVHAccel;
		template <typename T, typename HolderT>
		class BVHNode;

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
				Normalf n;
				Point2f texcoord;
				Normalf tangent;
			};

		public:
			RayIntersector();

			void Init(const Ray & ray);

		public:
			Rst & GetRst() { return rst; }

		private:
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<BVHAccel> bvhAccel);
			void Visit(Basic::Ptr<BVHNode<Basic::Element, BVHAccel>> bvhNode);
			void Visit(Basic::Ptr<SObj> sobj);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<Triangle> triangle);

		private:
			bool Intersect(const BBoxf & bbox);
			bool Intersect(const BBoxf & bbox, float & t0, float & t1);

		private:
			Ray ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_