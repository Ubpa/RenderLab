#pragma once

#include <Basic/HeapObj.h>
#include <UGM/UGM>

#include <UHEMesh/HEMesh.h>

namespace CppUtil {
	namespace Engine {
		class TriMesh;

		class IsotropicRemeshing : public Basic::HeapObj {
		public:
			IsotropicRemeshing(Basic::Ptr<TriMesh> triMesh);

		public:
			static const Basic::Ptr<IsotropicRemeshing> New(Basic::Ptr<TriMesh> triMesh) {
				return Basic::New<IsotropicRemeshing>(triMesh);
			}

		public:
			bool Init(Basic::Ptr<TriMesh> triMesh);
			void Clear();
			bool Run(size_t n);

		private:
			bool Kernel(size_t n);

		private:
			class V;
			class E;
			class V : public Ubpa::TVertex<V, E> {
			public:
				V(const Ubpa::vecf3 pos = 0.f) : pos(pos) {}
			public:
				const Ubpa::vecf3 Project(const Ubpa::vecf3& p, const Ubpa::normalf & norm) const;
			public:
				Ubpa::vecf3 pos;
				Ubpa::vecf3 newPos;
			};
			class E : public Ubpa::TEdge<V, E> {
			public:
				float Length() const { return (HalfEdge()->Origin()->pos - HalfEdge()->End()->pos).norm(); }
				Ubpa::vecf3 Centroid() const { return (HalfEdge()->Origin()->pos + HalfEdge()->End()->pos) / 2.f; }
				bool IsCanCollapse(float min, float maxL) const;
			};
		private:
			Basic::Ptr<TriMesh> triMesh;
			const Basic::Ptr<Ubpa::HEMesh<V>> heMesh;
		};
	}
}
