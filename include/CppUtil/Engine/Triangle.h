#ifndef _ENGINE_PRIMITIVE_TRIANGLE_H_
#define _ENGINE_PRIMITIVE_TRIANGLE_H_

#include <CppUtil/Basic/Element.h>
#include <CppUtil/Engine/BBox.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		// 因为 Triangle 依赖于 Mesh，而且 Mesh 又可以取代 Triangle
		// 所以不把 Triangle 当作 Primitive，只让 Mesh 当作 Primitive
		class Triangle : public Basic::Element {
			ELE_SETUP(Triangle)
			friend class TriMesh;
		public:
			Triangle(size_t idx0, size_t idx1, size_t idx2)
				: mesh(Basic::Ptr<TriMesh>(nullptr)), idx{ idx0, idx1, idx2 } { }

			Basic::Ptr<TriMesh> GetMesh() const { return mesh.lock(); }
			const BBox GetBBox() const;
		public:
			size_t idx[3]; // index into the mesh attribute arrays

		private:
			Basic::WPtr<TriMesh> mesh;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_TRIANGLE_H_
