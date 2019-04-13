#ifndef _ENGINE_RTX_BVH_ACCEL_H_
#define _ENGINE_RTX_BVH_ACCEL_H_

#include <CppUtil/Engine/Shape.h>

#include <CppUtil/Basic/UGM/BBox.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <vector>
#include <unordered_map>

namespace CppUtil {
	namespace Basic {
		class Shapement;
	}

	namespace Engine {
		class SObj;
		class Primitive;
		class BVHNode;

		class BVHAccel : public Basic::Element {
		public:
			BVHAccel();

		public:
			static const Basic::Ptr<BVHAccel> New() { return Basic::New<BVHAccel>(); }

		protected:
			virtual ~BVHAccel() = default;

		public:
			void Init(Basic::Ptr<SObj> root);
			void Clear();

		public:
			Basic::Ptr<BVHNode> GetBVHRoot() const { return bvhRoot; }

			const Basic::Transform & GetShapeW2LMat(Basic::Ptr<Shape> shape) const;
			const Basic::Transform & GetShapeL2WMat(Basic::Ptr<Shape> shape) const;
			const Basic::Transform & GetSObjL2WMat(Basic::Ptr<SObj> sobj) const;
			const BBoxf & GetWorldBBox(Basic::Ptr<Shape> shape) const;
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Shape> shape) const;

		private:
			// bvh
			Basic::Ptr<BVHNode> bvhRoot;
			// triangle 要通过 mesh 在这里取得 matrix
			class GetPrimitiveVisitor;
			std::unordered_map<Basic::Ptr<Primitive>, Basic::Transform> worldToLocalMatrixes;
			std::unordered_map<Basic::Ptr<Primitive>, Basic::Transform> localToWorldMatrixes;
			std::unordered_map<Basic::Ptr<SObj>, Basic::Transform> sobjL2W;
			std::unordered_map<Basic::Ptr<Primitive>, Basic::Ptr<SObj>> primitive2sobj;

			// obj and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Shape>> shapes;
			std::unordered_map<Basic::Ptr<Shape>, BBoxf> shape2wbbox;

		};
	}
}

#endif//!_ENGINE_RTX_BVH_ACCEL_H_
