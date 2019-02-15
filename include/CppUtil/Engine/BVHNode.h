#ifndef _ENGINE_PRIMITIVE_BVH_NODE_H_
#define _ENGINE_PRIMITIVE_BVH_NODE_H_

#include <CppUtil/Basic/Element.h>
#include <CppUtil/Engine/BBox.h>

#include <vector>

namespace CppUtil {
	namespace Engine {

		template<typename T, typename HolderT>
		class BVHNode : public Basic::Element {
			ELE_SETUP(BVHNode)
		public:
			BVHNode(HolderT * holder, std::vector<Basic::Ptr<T>> & objs, size_t start, size_t range, size_t maxLeafSize = 4)
				: holder(holder), objs(objs), start(start), range(range) {
				Build(maxLeafSize);
			}

			bool IsLeaf() const { return l == nullptr && r == nullptr; }

			HolderT * GetHolder()const { return holder; }
			const std::vector<Basic::Ptr<T>> & GetObjs() const { return objs; }
			const BBox & GetBBox() const { return bb; }
			size_t GetStart() const { return start; }
			size_t GetRange() const { return range; }
			BVHNode::Ptr GetL() const { return l; }
			BVHNode::Ptr GetR() const { return r; }

			// 可通过 模板实例化 来自行定义
			const BBox GetBBox(Basic::Ptr<T> obj) const {
				return holder->GetBBox(obj);
			}

		private:
			// 要先设置好 start, range 和 triangles
			// 调整 triangles 的元素顺序，设置 l, r 和 bb
			void Build(size_t maxLeafSize);

		private:
			HolderT * holder;
			std::vector<Basic::Ptr<T>> & objs;
			BBox bb;       // bounding box of the node
			size_t start;  // start index into the primitive list
			size_t range;  // range of index into the primitive list
			BVHNode::Ptr l;    // left child node
			BVHNode::Ptr r;    // right child node
		};

#include <CppUtil/Engine/BVHNode.inl>
	}
}

#endif//!_ENGINE_PRIMITIVE_BVH_NODE_H_
