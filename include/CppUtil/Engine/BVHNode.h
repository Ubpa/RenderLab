#ifndef _CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_
#define _CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_

#include <CppUtil/Engine/Shape.h>
#include <CppUtil/Basic/UGM/BBox.h>

#include <vector>

namespace CppUtil {
	namespace Engine {
		class BVHAccel;

		class BVHNode final : public Basic::Element {
		public:
			BVHNode(BVHAccel * holder, std::vector<Basic::Ptr<Shape>> & shapes, size_t start, size_t range, size_t maxLeafSize = 4)
				: holder(holder), shapes(shapes), start(start), range(range)
			{
				Build(maxLeafSize);
			}

		public:
			static const Basic::Ptr<BVHNode> New(BVHAccel * holder, std::vector<Basic::Ptr<Shape>> & shapes, size_t start, size_t range, size_t maxLeafSize = 4) {
				return Basic::New<BVHNode>(holder, shapes, start, range, maxLeafSize);
			}

		private:
			virtual ~BVHNode() = default;

		public:
			bool IsLeaf() const { return l == nullptr && r == nullptr; }

			BVHAccel * GetHolder() const { return holder; }
			const std::vector<Basic::Ptr<Shape>> & GetShapes() const { return shapes; }
			const BBoxf & GetBBox() const { return bb; }
			size_t GetStart() const { return start; }
			size_t GetRange() const { return range; }
			Basic::Ptr<BVHNode> GetL() const { return l; }
			Basic::Ptr<BVHNode> GetR() const { return r; }

		private:
			// 要先设置好 start, range 和 triangles
			// 调整 triangles 的元素顺序，设置 l, r 和 bb
			void Build(size_t maxLeafSize);

		private:
			BVHAccel * holder;
			std::vector<Basic::Ptr<Shape>> & shapes;
			BBoxf bb;       // bounding box of the node
			size_t start;  // start index into the primitive list
			size_t range;  // range of index into the primitive list
			Basic::Ptr<BVHNode> l;    // left child node
			Basic::Ptr<BVHNode> r;    // right child node
		};
	}
}

#endif//!_CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_
