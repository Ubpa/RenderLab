#pragma once

#include <Basic/HeapObj.h>
#include <UGM/UGM>

#include <UHEMesh/HEMesh.h>

namespace Ubpa {
	class TriMesh;

	class IsotropicRemeshing : public HeapObj {
	public:
		IsotropicRemeshing(Ptr<TriMesh> triMesh);

	public:
		static const Ptr<IsotropicRemeshing> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<IsotropicRemeshing>(triMesh);
		}

	public:
		bool Init(Ptr<TriMesh> triMesh);
		void Clear();
		bool Run(size_t n);

	private:
		bool Kernel(size_t n);

	private:
		class V;
		class E;
		class V : public TVertex<V, E> {
		public:
			V(const vecf3 pos = 0.f) : pos(pos) {}
		public:
			const vecf3 Project(const vecf3& p, const normalf& norm) const;
		public:
			vecf3 pos;
			vecf3 newPos;
		};
		class E : public TEdge<V, E> {
		public:
			float Length() const { return (HalfEdge()->Origin()->pos - HalfEdge()->End()->pos).norm(); }
			vecf3 Centroid() const { return (HalfEdge()->Origin()->pos + HalfEdge()->End()->pos) / 2.f; }
			bool IsCanCollapse(float min, float maxL) const;
		};
	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh;
	};
}
