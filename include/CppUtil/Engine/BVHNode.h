#ifndef _ENGINE_PRIMITIVE_BVH_NODE_H_
#define _ENGINE_PRIMITIVE_BVH_NODE_H_

#include <CppUtil/Basic/Element.h>
#include <CppUtil/Basic/UGM/BBox.h>

#include <vector>

namespace CppUtil {
	namespace Engine {

		template<typename T, typename HolderT>
		class BVHNode final : public Basic::Element<BVHNode<T,HolderT>> {
		public:
			BVHNode(HolderT * holder, std::vector<Basic::Ptr<T>> & objs, size_t start, size_t range, size_t maxLeafSize = 4)
				: holder(holder), objs(objs), start(start), range(range)
			{
				Build(maxLeafSize);
			}

		public:
			static const Basic::Ptr<BVHNode> New(HolderT * holder, std::vector<Basic::Ptr<T>> & objs, size_t start, size_t range, size_t maxLeafSize = 4) {
				return Basic::New<BVHNode>(holder, objs, start, range, maxLeafSize);
			}

		private:
			virtual ~BVHNode() = default;

		public:
			bool IsLeaf() const { return l == nullptr && r == nullptr; }

			HolderT * GetHolder()const { return holder; }
			const std::vector<Basic::Ptr<T>> & GetObjs() const { return objs; }
			const BBoxf & GetBBox() const { return bb; }
			size_t GetStart() const { return start; }
			size_t GetRange() const { return range; }
			Basic::Ptr<BVHNode> GetL() const { return l; }
			Basic::Ptr<BVHNode> GetR() const { return r; }

			// 可通过 模板实例化 来自行定义
			const BBoxf GetBBox(Basic::Ptr<T> obj) const {
				return holder->GetBBox(obj);
			}

		private:
			// 要先设置好 start, range 和 triangles
			// 调整 triangles 的元素顺序，设置 l, r 和 bb
			void Build(size_t maxLeafSize);

		private:
			HolderT * holder;
			std::vector<Basic::Ptr<T>> & objs;
			BBoxf bb;       // bounding box of the node
			size_t start;  // start index into the primitive list
			size_t range;  // range of index into the primitive list
			Basic::Ptr<BVHNode> l;    // left child node
			Basic::Ptr<BVHNode> r;    // right child node
		};

#include <CppUtil/Engine/BVHNode.inl>
	}
}

#endif//!_ENGINE_PRIMITIVE_BVH_NODE_H_
