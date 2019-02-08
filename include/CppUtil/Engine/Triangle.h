#ifndef _ENGINE_TRIANGLE_H_
#define _ENGINE_TRIANGLE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Engine/BBox.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class Triangle : public Basic::HeapObj {
			HEAP_OBJ_SETUP(Triangle)
			friend class TriMesh;
		public:
			Triangle(size_t idx0, size_t idx1, size_t idx2)
				: mesh(Basic::Ptr<TriMesh>(nullptr)), idx{ idx0, idx1, idx2 } { }

			Basic::Ptr<TriMesh> GetMesh() const { return mesh.lock(); }
			BBox GetBBox() const;
		public:
			size_t idx[3]; // index into the mesh attribute arrays

		private:
			Basic::WPtr<TriMesh> mesh;
		};
	}
}

#endif//!_ENGINE_TRIANGLE_H_
