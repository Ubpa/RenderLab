#ifndef _ENGINE_PRIMITIVE_TRIANGLE_H_
#define _ENGINE_PRIMITIVE_TRIANGLE_H_

typedef unsigned int uint;

#include <Engine/Shape.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		// 因为 Triangle 依赖于 Mesh，而且 Mesh 又可以取代 Triangle
		// 所以不把 Triangle 当作 Primitive，只让 Mesh 当作 Primitive
		class Triangle final : public Shape {
			friend class TriMesh;
		public:
			Triangle(uint idx0, uint idx1, uint idx2)
				: mesh(Basic::Ptr<TriMesh>(nullptr)), idx{ idx0, idx1, idx2 } { }

		public:
			static const Basic::Ptr<Triangle> New(uint idx0, uint idx1, uint idx2) {
				return Basic::New<Triangle>(idx0, idx1, idx2);
			}

		protected:
			virtual ~Triangle() = default;

		public:
			const Basic::Ptr<TriMesh> GetMesh() const { return mesh.lock(); }
			virtual const BBoxf GetBBox() const override;
			virtual const Basic::Ptr<Primitive> GetPrimitive() override {
				return Basic::CastTo<Primitive>(mesh.lock());
			}

		public:
			float GetArea() const;

		public:
			uint idx[3]; // index into the mesh attribute arrays

		private:
			Basic::WPtr<TriMesh> mesh;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_TRIANGLE_H_
