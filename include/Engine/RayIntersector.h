#pragma once

#include <Engine/Intersector.h>
#include <UGM/point.h>
#include <UGM/point.h>
#include <UGM/normal.h>
#include <UGM/bbox.h>

#include <Engine/Ray.h>

namespace CppUtil {
	namespace Basic {
		class Element;
	}

	namespace Engine {
		// 寻找最近的交点
		class RayIntersector final : public Intersector {
		public:
			// isIntersect 用于判断与 Primitive 是否相交
			// closestSObj 用于记录最近的 SObj
			// n 用于记录最近的相交处的法向
			struct Rst {
				bool IsIntersect() const {
					return isIntersect;
				}

				Basic::Ptr<SObj> closestSObj;
				Ubpa::normalf n;
				Ubpa::pointf2 texcoord;
				Ubpa::normalf tangent;
				Basic::Ptr<Triangle> triangle;
				int idx;
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
			void Visit(Basic::Ptr<Capsule> capsule);

		private:
			bool Intersect(const Ubpa::bboxf3 & bbox, const Ubpa::valf3 & invDir) const;

		private:
			Ray * ray;
			Rst rst;
		};
	}
}
