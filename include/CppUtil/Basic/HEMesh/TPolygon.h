#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
#define _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge;

		template<typename V, typename E, typename P>
		class TPolygon : public HeapObj {
		private:
			using HE = THalfEdge<V, E, P>;

		public:
			const Ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge.lock(); }
			const PtrC<THalfEdge<V, E, P>> HalfEdge() const { return const_cast<TPolygon*>(this)->HalfEdge(); }

			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			const std::vector<Ptr<HE>> BoundaryHEs();
			const std::vector<PtrC<HE>> BoundaryHEs() const { return Const(const_cast<TPolygon*>(this)->BoundaryHEs()); }
			size_t Degree() const { return static_cast<int>(BoundaryHEs().size()); }
			const std::vector<Ptr<E>> BoundaryEdges();
			const std::vector<PtrC<E>> BoundaryEdges() const { return Const(const_cast<TPolygon*>(this)->BoundaryEdges()); }

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

#include <CppUtil/Basic/HEMesh/TPolygon.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
