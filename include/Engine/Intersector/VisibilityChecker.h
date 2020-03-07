#pragma once

#include <Engine/Intersector/Intersector.h>
#include <UGM/bbox.h>

#include <Basic/Ptr.h>
#include <Basic/HeapObj.h>
#include <Engine/Primitive/Shape.h>

#include <Engine/Viewer/Ray.h>

namespace Ubpa {
	class VisibilityChecker final : public SharedPtrVisitor<VisibilityChecker, Shape>, public HeapObj, public Intersector {
	public:
		struct Rst {
			friend class VisibilityChecker;

			bool IsIntersect() const { return isIntersect; }

		private:
			bool isIntersect;
		};

	public:
		VisibilityChecker();

	public:
		static const Ptr<VisibilityChecker> New() { return Ubpa::New<VisibilityChecker>(); }

	protected:
		virtual ~VisibilityChecker() = default;

	public:
		void Init(const Ray& ray, float tMax);

		using SharedPtrVisitor<VisibilityChecker, Shape>::Visit;
		void Visit(Ptr<BVHAccel> bvhAccel);
		Rst& GetRst() { return rst; }

	protected:
		// 设置 rst，如果相交，则会修改 ray.tMax
		void ImplVisit(Ptr<Sphere> sphere);
		void ImplVisit(Ptr<Plane> plane);
		void ImplVisit(Ptr<Triangle> triangle);
		void ImplVisit(Ptr<Disk> disk);
		void ImplVisit(Ptr<Capsule> capsule);

	private:
		bool Intersect(const bboxf3& bbox, const valf3& invDir) const;

	private:
		Ray ray;
		Rst rst;
	};
}
