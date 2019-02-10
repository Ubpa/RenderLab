#ifndef _ENGINE_PRIMITIVE_BVH_NODE_H_
#define _ENGINE_PRIMITIVE_BVH_NODE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Engine/BBox.h>

#include <vector>

namespace CppUtil {
	namespace Engine {
		// T 应该要有 GetBBox 函数
		template<typename T>
		class BVHNode : public Basic::HeapObj {
			HEAP_OBJ_SETUP(BVHNode<T>)
		public:
			BVHNode(std::vector<Basic::Ptr<T>> & boxObjs, size_t maxLeafSize, size_t start, size_t range)
				: boxObjs(boxObjs), start(start), range(range) {
				Build(maxLeafSize);
			}

			bool IsLeaf() const { return l == nullptr && r == nullptr; }

			const std::vector<Basic::Ptr<T>> & GetBoxObjs() const { return boxObjs; }
			const BBox & GetBBox() const { return bb; }
			size_t GetStart() const { return start; }
			size_t GetRange() const { return range; }
			BVHNode::Ptr GetL() const { return l; }
			BVHNode::Ptr GetR() const { return r; }

		private:
			// 要先设置好 start, range 和 triangles
			// 调整 triangles 的元素顺序，设置 l, r 和 bb
			void Build(size_t maxLeafSize);

		private:
			std::vector<Basic::Ptr<T>> & boxObjs;
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
