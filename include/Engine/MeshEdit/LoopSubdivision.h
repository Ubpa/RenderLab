#pragma once

#include <Basic/HeapObj.h>
#include <UGM/UGM>
#include <UHEMesh/HEMesh.h>

namespace Ubpa {
	class TriMesh;

	class LoopSubdivision : public HeapObj {
	public:
		LoopSubdivision(Ptr<TriMesh> triMesh);

	public:
		static const Ptr<LoopSubdivision> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<LoopSubdivision>(triMesh);
		}

	public:
		bool Init(Ptr<TriMesh> triMesh);
		void Clear();
		bool Run(size_t n);

	private:
		void Kernel();

	private:
		class V;
		class E;
		class V : public TVertex<V, E> {
		public:
			vecf3 pos;
			bool isNew = false;
			vecf3 newPos;
		};
		class E : public TEdge<V, E> {
		public:
			vecf3 newPos;
		};
	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;
	};
}
