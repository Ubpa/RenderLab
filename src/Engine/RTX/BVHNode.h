#pragma once

#include <Engine/Primitive/Shape.h>
#include <UGM/bbox.h>

#include <vector>
#include <unordered_map>

namespace Ubpa {
	class BVHNode final : public HeapObj {
	public:
		BVHNode(const std::unordered_map<Ptr<Shape>, bboxf3>& shape2wbbox, std::vector<Ptr<Shape>>& shapes, size_t shapesOffset, size_t shapesNum)
			: shapesOffset(shapesOffset), shapesNum(shapesNum) {
			Build(shape2wbbox, shapes);
		}

	public:
		static const Ptr<BVHNode> New(const std::unordered_map<Ptr<Shape>, bboxf3>& shape2wbbox, std::vector<Ptr<Shape>>& shapes, size_t shapesOffset, size_t shapesNum) {
			return Ubpa::New<BVHNode>(shape2wbbox, shapes, shapesOffset, shapesNum);
		}

	private:
		virtual ~BVHNode() = default;

	public:
		bool IsLeaf() const { return l == nullptr && r == nullptr; }

		const bboxf3& GetBBox() const { return box; }
		size_t GetShapeOffset() const { return shapesOffset; }
		size_t GetShapesNum() const { return shapesNum; }
		int GetAxis() const { return axis; }
		Ptr<BVHNode> GetL() const { return l; }
		Ptr<BVHNode> GetR() const { return r; }

	private:
		// 调整 shapes 的元素顺序，设置 l, r, box 和 axis
		void Build(const std::unordered_map<Ptr<Shape>, bboxf3>& shape2wbbox, std::vector<Ptr<Shape>>& shapes);

	private:
		bboxf3 box;
		size_t shapesOffset;
		size_t shapesNum;
		int axis{ -1 };
		Ptr<BVHNode> l;
		Ptr<BVHNode> r;
	};
}
