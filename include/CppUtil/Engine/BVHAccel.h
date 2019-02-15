#ifndef _ENGINE_RTX_BVH_ACCEL_H_
#define _ENGINE_RTX_BVH_ACCEL_H_

#include <CppUtil/Basic/Element.h>

#include <CppUtil/Engine/BBox.h>

#include <glm/mat3x3.hpp>

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
			const glm::mat4 & GetEleW2LMat(Basic::Ptr<Basic::Element> element);
			const glm::mat4 & GetEleL2WMat(Basic::Ptr<Basic::Element> element);
			const glm::mat3 & GetNormL2WMat(Basic::Ptr<SObj> sobj) { return norm_localToWorld[sobj]; }
			const BBox & GetBBox(Basic::Ptr<Basic::Element> element) { return ele2bbox[element]; }
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Basic::Element> element);

		private:
			// bvh
			std::map<Basic::Ptr<SObj>, glm::mat3> norm_localToWorld;
			Basic::Ptr<BVHNode<Basic::Element, BVHAccel>> bvhRoot;
			// triangle 要通过 mesh 在这里取得 matrix
			class GetPrimitiveVisitor;
			std::map<Basic::Ptr<Primitive>, glm::mat4> worldToLocalMatrixes;
			std::map<Basic::Ptr<Primitive>, glm::mat4> localToWorldMatrixes;
			std::map<Basic::Ptr<Primitive>, Basic::Ptr<SObj>> primitive2sobj;

			// obj and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Basic::Element>> elements;
			std::map<Basic::Ptr<Basic::Element>, BBox> ele2bbox;

		};
	}
}

#endif//!_ENGINE_RTX_BVH_ACCEL_H_
