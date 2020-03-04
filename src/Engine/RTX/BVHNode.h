#ifndef _CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_
#define _CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_

#include <Engine/Shape.h>
#include <Basic/UGM/BBox.h>

#include <vector>
#include <unordered_map>

namespace CppUtil {
	namespace Engine {
		class BVHNode final : public Basic::Element {
		public:
			BVHNode(const std::unordered_map<Basic::Ptr<Shape>,BBoxf> & shape2wbbox, std::vector<Basic::Ptr<Shape>> & shapes, size_t shapesOffset, size_t shapesNum)
				: shapesOffset(shapesOffset), shapesNum(shapesNum) { Build(shape2wbbox, shapes); }

		public:
			static const Basic::Ptr<BVHNode> New(const std::unordered_map<Basic::Ptr<Shape>, BBoxf> & shape2wbbox, std::vector<Basic::Ptr<Shape>> & shapes, size_t shapesOffset, size_t shapesNum) {
				return Basic::New<BVHNode>(shape2wbbox, shapes, shapesOffset, shapesNum);
			}

		private:
			virtual ~BVHNode() = default;

		public:
			bool IsLeaf() const { return l == nullptr && r == nullptr; }

			const BBoxf & GetBBox() const { return box; }
			size_t GetShapeOffset() const { return shapesOffset; }
			size_t GetShapesNum() const { return shapesNum; }
			int GetAxis() const { return axis; }
			Basic::Ptr<BVHNode> GetL() const { return l; }
			Basic::Ptr<BVHNode> GetR() const { return r; }

		private:
			// 调整 shapes 的元素顺序，设置 l, r, box 和 axis
			void Build(const std::unordered_map<Basic::Ptr<Shape>, BBoxf> & shape2wbbox, std::vector<Basic::Ptr<Shape>> & shapes);

		private:
			BBoxf box;
			size_t shapesOffset;
			size_t shapesNum;
			int axis{-1};
			Basic::Ptr<BVHNode> l;
			Basic::Ptr<BVHNode> r;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_PRIMITIVE_BVH_NODE_H_
