#pragma once

#include <Engine/Intersector.h>
#include <UGM/bbox.h>

#include <Engine/Ray.h>

namespace CppUtil {
	namespace Engine {
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
			static const Basic::Ptr<VisibilityChecker> New() { return Basic::New<VisibilityChecker>(); }

		protected:
			virtual ~VisibilityChecker() = default;

		public:
			void Init(const Ray & ray, float tMax);

			Rst & GetRst() { return rst; }

		private:
			// 设置 rst，如果相交，则会修改 ray.tMax
			void Visit(Basic::Ptr<BVHAccel> bvhAccel);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<Triangle> triangle);
			void Visit(Basic::Ptr<Disk> disk);
			void Visit(Basic::Ptr<Capsule> capsule);

		private:
			bool Intersect(const Ubpa::bboxf3 & bbox, const Ubpa::valf3 & invDir) const;

		private:
			Ray ray;
			Rst rst;
		};
	}
}
