#ifndef _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_

#include <CppUtil/Engine/Intersector.h>
#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/Point2.h>
#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/BBox.h>

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
		class TriMesh;
		class BVHAccel;
		class Disk;

		// 寻找最近的交点
		class RayIntersector final : public Intersector {
		public:
			// isIntersect 用于判断与 Primitive 是否相交
			// closestSObj 用于记录最近的SObj
			// n 用于记录最近的相交处的法向
			struct Rst {
				bool IsIntersect() const {
					return closestSObj != nullptr;
				}

				Basic::Ptr<SObj> closestSObj;
				Normalf n;
				Point2 texcoord;
				Normalf tangent;
			private:
				friend class RayIntersector;
				bool isIntersect;
			};

		public:
			RayIntersector();

			void Init(Ray * ray);

		protected:
			virtual ~RayIntersector() = default;
			
		public:
			static const Basic::Ptr<RayIntersector> New() { return Basic::New<RayIntersector>(); }

		public:
			const Rst & GetRst() { return rst; }

		private:
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<BVHAccel> bvhAccel);
			void Visit(Basic::Ptr<SObj> sobj);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<Triangle> triangle);
			void Visit(Basic::Ptr<TriMesh> mesh);
			void Visit(Basic::Ptr<Disk> disk);

		private:
			bool Intersect(const BBoxf & bbox, const Val3f & invDir) const;

		private:
			Ray * ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_RAY_INTERSECTOR_H_