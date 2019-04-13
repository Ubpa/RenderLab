#ifndef _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
#define _ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_

#include <CppUtil/Engine/Intersector.h>

#include <CppUtil/Engine/Ray.h>

namespace CppUtil {
	namespace Basic {
		class Element;

		template <typename T>
		class BBox;
	}

	namespace Engine {
		class SObj;
		class Ray;
		class Sphere;
		class Plane;
		class Triangle;
		class BVHAccel;
		class BVHNode;

		class VisibilityChecker final : public Intersector {
		public:
			struct Rst : public Intersector::Rst {
				friend class VisibilityChecker;

				Rst(bool isIntersect = false)
					: Intersector::Rst(isIntersect) { }

				bool IsIntersect() const { return isIntersect; }
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
			void Visit(Basic::Ptr<BVHNode> bvhNode);
			void Visit(Basic::Ptr<Sphere> sphere);
			void Visit(Basic::Ptr<Plane> plane);
			void Visit(Basic::Ptr<Triangle> triangle);

		private:
			bool Intersect(const Basic::BBox<float> & bbox);
			bool Intersect(const Basic::BBox<float> & bbox, float & t0, float & t1);

		private:
			Ray ray;
			Rst rst;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_VISIBILITY_CHECKER_H_
