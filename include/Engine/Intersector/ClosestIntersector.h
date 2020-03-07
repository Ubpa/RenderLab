#pragma once

#include "Intersector.h"

#include <Basic/Ptr.h>
#include <Basic/HeapObj.h>
#include <Engine/Primitive/Shape.h>

#include <UGM/point.h>
#include <UGM/point.h>
#include <UGM/normal.h>
#include <UGM/bbox.h>

#include <Engine/Viewer/Ray.h>

namespace Ubpa {
	class Element;

	// 寻找最近的交点
	class ClosestIntersector final : public HeapObj, public SharedPtrVisitor<ClosestIntersector, Shape>, public Intersector {
	public:
		// isIntersect 用于判断与 Primitive 是否相交
		// closestSObj 用于记录最近的 SObj
		// n 用于记录最近的相交处的法向
		struct Rst {
			bool IsIntersect() const {
				return isIntersect;
			}

			Ptr<SObj> closestSObj;
			normalf n;
			pointf2 texcoord;
			normalf tangent;
			Ptr<Triangle> triangle;
			int idx;
		private:
			friend class ClosestIntersector;
			bool isIntersect;
		};

	public:
		ClosestIntersector();

		void Init(Ray* ray);

		using SharedPtrVisitor<ClosestIntersector, Shape>::Visit;
		void Visit(Ptr<BVHAccel> bvhAccel);
		void Visit(Ptr<SObj> sobj);

	protected:
		virtual ~ClosestIntersector() = default;

	public:
		static const Ptr<ClosestIntersector> New() { return Ubpa::New<ClosestIntersector>(); }

	public:
		const Rst& GetRst() { return rst; }

	protected:
		// 设置 rst，如果相交，则会修改 ray.tMax
		void ImplVisit(Ptr<Sphere> sphere);
		void ImplVisit(Ptr<Plane> plane);
		void ImplVisit(Ptr<Triangle> triangle);
		void ImplVisit(Ptr<TriMesh> mesh);
		void ImplVisit(Ptr<Disk> disk);
		void ImplVisit(Ptr<Capsule> capsule);

	private:
		bool Intersect(const bboxf3& bbox, const valf3& invDir) const;

	private:
		Ray* ray;
		Rst rst;
	};
}
