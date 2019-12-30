#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <CppUtil/Basic/HEMesh/TVertex.h>
#include <CppUtil/Basic/HEMesh/TEdge.h>
#include <CppUtil/Basic/HEMesh/TPolygon.h>

#include <vector>
#include <unordered_set>
#include <assert.h>

namespace CppUtil {
	namespace Basic {
		template<typename V,
			typename = std::enable_if_t<std::is_base_of_v<TVertex<V, typename V::E_t, typename V::P_t>, V>>,
			typename = std::enable_if_t<std::is_base_of_v<TEdge<V, typename V::E_t, typename V::P_t>, typename V::E_t>>,
			typename = std::enable_if_t<std::is_base_of_v<TPolygon<V, typename V::E_t, typename V::P_t>, typename V::P_t>>
		>
		class HEMesh : public HeapObj {
		public:
			using HE = typename V::HE;
			using E = typename V::E_t;
			using P = typename V::P_t;

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }

		public:
			const std::unordered_set<Ptr<V>> & Vertices() { return vertices; }
			const std::unordered_set<Ptr<HE>> & HalfEdges() { return halfEdges; }
			const std::unordered_set<Ptr<E>> & Edges() { return edges; }
			const std::unordered_set<Ptr<P>> & Polygons() { return polygons; }

			const std::unordered_set<PtrC<V>> Vertices() const { return Const(vertices); }
			const std::unordered_set<PtrC<HE>> HalfEdges() const { return Const(halfEdges); }
			const std::unordered_set<PtrC<E>> Edges() const { return Const(edges); }
			const std::unordered_set<PtrC<P>> Polygons() const { return Const(polygons); }

			int NumVertices() const { return vertices.size(); }
			int NumEdges() const { return halfEdges.size(); }
			int NumPolygons() const { return polygons.size(); }
			int NumHalfEdges() const { return halfEdges.size(); }

			const Ptr<V> AddVertex();
			// e's halfedge is form v0 to v1
			const Ptr<E> AddEdge(Ptr<V> v0, Ptr<V> v1, Ptr<E> e = Basic::New<E>());
			// polygon's halfedge is heLoop[0]
			const Ptr<P> AddPolygon(const std::vector<Ptr<HE>> heLoop);
			void RemovePolygon(Ptr<P> polygon);
			void RemoveEdge(Ptr<E> e);
			void RemoveVertex(Ptr<V> v);

			Ptr<V> SpiltEdge(Ptr<E> e);
			bool FlipEdge(Ptr<E> e);

			void Clear();

		protected:
			virtual ~HEMesh() = default;

		private:
			std::unordered_set<Ptr<V>> vertices;
			std::unordered_set<Ptr<HE>> halfEdges;
			std::unordered_set<Ptr<E>> edges;
			std::unordered_set<Ptr<P>> polygons;
		};

#include<CppUtil/Basic/HEMesh/HEMesh.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
