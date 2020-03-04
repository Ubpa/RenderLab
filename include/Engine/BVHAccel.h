#ifndef _ENGINE_RTX_BVH_ACCEL_H_
#define _ENGINE_RTX_BVH_ACCEL_H_

#include <Basic/Element.h>

#include <Basic/UGM/BBox.h>
#include <Basic/UGM/Transform.h>

#include <vector>
#include <unordered_map>

namespace CppUtil {
	namespace Engine {
		class Shape;
		class Primitive;
		class SObj;

		class BVHNode;

		class BVHAccel : public Basic::Element {
		public:
			BVHAccel() = default;

		public:
			static const Basic::Ptr<BVHAccel> New() { return Basic::New<BVHAccel>(); }

		protected:
			virtual ~BVHAccel() = default;

		public:
			class LinearBVHNode {
			public:
				void InitLeaf(const BBoxf & box, int shapesOffset, int shapesNum) {
					this->box = box;
					this->shapesOffset = shapesOffset;
					this->shapesNum = shapesNum;
					axis = -1;
				}

				void InitBranch(const BBoxf & box, int secondChildIdx, int axis) {
					this->box = box;
					this->secondChildIdx = secondChildIdx;
					shapesNum = 0;
					this->axis = axis;
				}

			public:
				const BBoxf & GetBox() const { return box; }
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
				int GetAxis() const{
					assert(!IsLeaf());
					return axis;
				}

			private:
				BBoxf box;
				union {
					int shapesOffset; // leaf
					int secondChildIdx; // interior
				};
				uint16_t shapesNum; // 0 -> interior node
				uint8_t axis; // interior node: xyz
				const uint8_t pad[1]{ 0 }; // ensure 32 byte total size
			};

		public:
			void Init(Basic::Ptr<SObj> root);
			void Clear();

		public:
			const Basic::Transform & GetShapeW2LMat(Basic::Ptr<Shape> shape) const;
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Shape> shape) const;

			const LinearBVHNode & GetBVHNode(int idx) const {
				assert(idx >= 0 && idx < linearBVHNodes.size());
				return linearBVHNodes[idx];
			}
			const Basic::Ptr<Shape> GetShape(int idx) const {
				assert(idx >= 0 && idx < shapes.size());
				return shapes[idx];
			}

		private:
			void LinearizeBVH(Basic::Ptr<BVHNode> bvhNode);

		private:
			// triangle 要通过 mesh 在这里取得 matrix
			std::unordered_map<Basic::Ptr<Primitive>, Basic::Transform> worldToLocalMatrixes;
			std::unordered_map<Basic::Ptr<Primitive>, Basic::Ptr<SObj>> primitive2sobj;

			// shapes and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Shape>> shapes;

			std::vector<LinearBVHNode> linearBVHNodes;
		};
	}
}

#endif//!_ENGINE_RTX_BVH_ACCEL_H_
