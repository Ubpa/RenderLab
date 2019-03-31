#ifndef _ENGINE_RTX_BVH_ACCEL_H_
#define _ENGINE_RTX_BVH_ACCEL_H_

#include <CppUtil/Basic/Element.h>

#include <CppUtil/Basic/BBox.h>
#include <CppUtil/Basic/Transform.h>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		class Element;
	}

	namespace Engine {
		class SObj;
		class Primitive;
		template <typename T, typename HolderT>
		class BVHNode;
		class Intersector;

		class BVHAccel : public Basic::Element {
			ELE_SETUP(BVHAccel)
		public:
			BVHAccel();

			void Init(Basic::Ptr<SObj> root);

			Basic::Ptr<BVHNode<Basic::Element, BVHAccel>> GetBVHRoot() const { return bvhRoot; }
			const Basic::Transform & GetEleW2LMat(Basic::Ptr<Basic::Element> element);
			const Basic::Transform & GetEleL2WMat(Basic::Ptr<Basic::Element> element);
			const Basic::BBoxf & GetBBox(Basic::Ptr<Basic::Element> element) { return ele2bbox[element]; }
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Basic::Element> element);

		private:
			// bvh
			Basic::Ptr<BVHNode<Basic::Element, BVHAccel>> bvhRoot;
			// triangle 要通过 mesh 在这里取得 matrix
			class GetPrimitiveVisitor;
			std::map<Basic::Ptr<Primitive>, Basic::Transform> worldToLocalMatrixes;
			std::map<Basic::Ptr<Primitive>, Basic::Transform> localToWorldMatrixes;
			std::map<Basic::Ptr<Primitive>, Basic::Ptr<SObj>> primitive2sobj;

			// obj and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Basic::Element>> elements;
			std::map<Basic::Ptr<Basic::Element>, Basic::BBoxf> ele2bbox;

		};
	}
}

#endif//!_ENGINE_RTX_BVH_ACCEL_H_
