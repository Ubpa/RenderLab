#pragma once

#include <Engine/Intersector.h>
#include <UGM/bbox.h>

#include <Engine/Ray.h>

namespace Ubpa {
	class VisibilityChecker final : public Intersector {
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

		Rst& GetRst() { return rst; }

	private:
		// 设置 rst，如果相交，则会修改 ray.tMax
		void Visit(Ptr<BVHAccel> bvhAccel);
		void Visit(Ptr<Sphere> sphere);
		void Visit(Ptr<Plane> plane);
		void Visit(Ptr<Triangle> triangle);
		void Visit(Ptr<Disk> disk);
		void Visit(Ptr<Capsule> capsule);

	private:
		bool Intersect(const bboxf3& bbox, const valf3& invDir) const;

	private:
		Ray ray;
		Rst rst;
	};
}
