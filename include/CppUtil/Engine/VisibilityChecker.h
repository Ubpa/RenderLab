#ifndef _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
#define _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_

#include <CppUtil/Engine/Intersector.h>
#include <CppUtil/Basic/UGM/BBox.h>

#include <CppUtil/Engine/Ray.h>

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
			bool Intersect(const BBoxf & bbox, const Val3f & invDir) const;

		private:
			Ray ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
