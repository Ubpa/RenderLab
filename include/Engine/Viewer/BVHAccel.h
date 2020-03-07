#pragma once

#include <Basic/HeapObj.h>

#include <UGM/bbox.h>
#include <UGM/transform.h>

#include <vector>
#include <unordered_map>

namespace Ubpa {
	class Shape;
	class Primitive;
	class SObj;

	class BVHNode;

	class BVHAccel : public HeapObj {
	public:
		BVHAccel() = default;

	public:
		static const Ptr<BVHAccel> New() { return Ubpa::New<BVHAccel>(); }

	protected:
		virtual ~BVHAccel() = default;

	public:
		class LinearBVHNode {
		public:
			void InitLeaf(const bboxf3& box, int shapesOffset, int shapesNum) {
				this->box = box;
				this->shapesOffset = shapesOffset;
				this->shapesNum = shapesNum;
				axis = -1;
			}

			void InitBranch(const bboxf3& box, int secondChildIdx, int axis) {
				this->box = box;
				this->secondChildIdx = secondChildIdx;
				shapesNum = 0;
				this->axis = axis;
			}

		public:
			const bboxf3& GetBox() const { return box; }
			bool IsLeaf() const { return shapesNum != 0; }
			const std::vector<int> ShapesIdx() const {
				assert(IsLeaf());
				std::vector<int> rst;
				for (uint16_t i = 0; i < shapesNum; i++)
					rst.push_back(shapesOffset + i);
				return rst;
			}
			static int FirstChildIdx(int nodeIdx) { return nodeIdx + 1; }
			int GetSecondChildIdx() const {
				assert(!IsLeaf());
				return secondChildIdx;
			}
			int GetAxis() const {
				assert(!IsLeaf());
				return axis;
			}

		private:
			bboxf3 box;
			union {
				int shapesOffset; // leaf
				int secondChildIdx; // interior
			};
			uint16_t shapesNum; // 0 -> interior node
			uint8_t axis; // interior node: xyz
			const uint8_t pad[1]{ 0 }; // ensure 32 byte total size
		};

	public:
		void Init(Ptr<SObj> root);
		void Clear();

	public:
		const transformf& GetShapeW2LMat(Ptr<Shape> shape) const;
		const Ptr<SObj> GetSObj(Ptr<Shape> shape) const;

		const LinearBVHNode& GetBVHNode(int idx) const {
			assert(idx >= 0 && idx < linearBVHNodes.size());
			return linearBVHNodes[idx];
		}
		const Ptr<Shape> GetShape(int idx) const {
			assert(idx >= 0 && idx < shapes.size());
			return shapes[idx];
		}

	private:
		void LinearizeBVH(Ptr<BVHNode> bvhNode);

	private:
		// triangle 要通过 mesh 在这里取得 matrix
		std::unordered_map<Ptr<Primitive>, transformf> worldToLocalMatrixes;
		std::unordered_map<Ptr<Primitive>, Ptr<SObj>> primitive2sobj;

		// shapes and box
		class BVHInitVisitor;
		friend class BVHInitVisitor;
		std::vector<Ptr<Shape>> shapes;

		std::vector<LinearBVHNode> linearBVHNodes;
	};
}
