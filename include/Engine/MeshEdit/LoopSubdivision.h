#pragma once

#include <Basic/HeapObj.h>
#include <UGM/UGM>
#include <UHEMesh/HEMesh.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class LoopSubdivision : public Basic::HeapObj {
		public:
			LoopSubdivision(Basic::Ptr<TriMesh> triMesh);

		public:
			static const Basic::Ptr<LoopSubdivision> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<LoopSubdivision>(triMesh);
			}

		public:
			bool Init(Basic::Ptr<TriMesh> triMesh);
			void Clear();
			bool Run(size_t n);

		private:
			void Kernel();

		private:
			class V;
			class E;
			class V : public Ubpa::TVertex<V, E> {
			public:
				Ubpa::vecf3 pos;
				bool isNew = false;
				Ubpa::vecf3 newPos;
			};
			class E : public Ubpa::TEdge<V, E> {
			public:
				Ubpa::vecf3 newPos;
			};
		private:
			Basic::Ptr<TriMesh> triMesh;
			const Basic::Ptr<Ubpa::HEMesh<V>> heMesh;
		};
	}
}
