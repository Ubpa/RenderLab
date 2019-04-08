#ifndef _ENGINE_RTX_BVH_ACCEL_H_
#define _ENGINE_RTX_BVH_ACCEL_H_

#include <CppUtil/Basic/Element.h>

#include <CppUtil/Basic/UGM/BBox.h>
#include <CppUtil/Basic/UGM/Transform.h>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		class ElementBase;
	}

	namespace Engine {
		class SObj;
		class PrimitiveBase;
		template <typename T, typename HolderT>
		class BVHNode;

		class BVHAccel : public Basic::Element<BVHAccel> {
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
			Basic::Ptr<BVHNode<Basic::ElementBase, BVHAccel>> GetBVHRoot() const { return bvhRoot; }

			const Basic::Transform & GetEleW2LMat(Basic::Ptr<Basic::ElementBase> element) const;
			const Basic::Transform & GetEleL2WMat(Basic::Ptr<Basic::ElementBase> element) const;
			const Basic::Transform & GetSObjL2WMat(Basic::Ptr<SObj> sobj) const;
			const BBoxf & GetBBox(Basic::Ptr<Basic::ElementBase> element) const;
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Basic::ElementBase> element) const;

		private:
			// bvh
			Basic::Ptr<BVHNode<Basic::ElementBase, BVHAccel>> bvhRoot;
			// triangle 要通过 mesh 在这里取得 matrix
			class GetPrimitiveVisitor;
			std::map<Basic::Ptr<PrimitiveBase>, Basic::Transform> worldToLocalMatrixes;
			std::map<Basic::Ptr<PrimitiveBase>, Basic::Transform> localToWorldMatrixes;
			std::map<Basic::Ptr<SObj>, Basic::Transform> sobjL2W;
			std::map<Basic::Ptr<PrimitiveBase>, Basic::Ptr<SObj>> primitive2sobj;

			// obj and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Basic::ElementBase>> elements;
			std::map<Basic::Ptr<Basic::ElementBase>, BBoxf> ele2bbox;

		};
	}
}

#endif//!_ENGINE_RTX_BVH_ACCEL_H_
