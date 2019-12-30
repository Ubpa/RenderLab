#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_

#include <CppUtil/Basic/HEMesh/TVertex.h>
#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <CppUtil/Basic/HEMesh/TEdge.h>
#include <CppUtil/Basic/HEMesh/TPolygon.h>

#include <vector>
#include <unordered_set>

namespace CppUtil {
	namespace Basic {
		template<typename V,
			typename = std::enable_if_t<std::is_base_of_v<TVertex<V, typename V::E_t, typename V::P_t>, V>>,
			typename = std::enable_if_t<std::is_base_of_v<TEdge<V, typename V::E_t, typename V::P_t>, typename V::E_t>>,
			typename = std::enable_if_t<std::is_base_of_v<TPolygon<V, typename V::E_t, typename V::P_t>, typename V::P_t>>
		>
		class HEMesh : public HeapObj {
		public:
			using E = typename V::E_t;
			using P = typename V::P_t;
			using HE = typename V::HE;

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }

		public:
			const std::unordered_set<Ptr<V>> & Vertices() { return vertices; }
			const std::unordered_set<Ptr<HE>> & HalfEdges() { return halfEdges; }
			const std::unordered_set<Ptr<E>> & Edges() { return edges; }
			const std::unordered_set<Ptr<P>> & Polygons() { return polygons; }

			const Ptr<V> AddVertex();
			const Ptr<E> AddEdge(Ptr<V> v0, Ptr<V> v1);
			const Ptr<P> AddPolygon(const std::vector<Ptr<HE>> heLoop);
			void RemovePolygon(Ptr<P> polygon);
			void RemoveEdge(Ptr<E> e);
			void RemoveVertex(Ptr<V> v);

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
