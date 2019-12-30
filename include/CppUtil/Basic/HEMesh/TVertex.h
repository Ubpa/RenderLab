#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_VERETX_H_
#define _CPPUTIL_BASIC_HEMESH_T_VERETX_H_

#include <CppUtil/Basic/HeapObj.h>
#include <vector>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge;

		template<typename V, typename E, typename P>
		class TVertex : public HeapObj {
		public:
			using E_t = E;
			using P_t = P;
			using HE = THalfEdge<V, E, P>;

		public:
			const Ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge.lock(); }
			const PtrC<THalfEdge<V, E, P>> HalfEdge() const { return const_cast<TVertex*>(this)->HalfEdge(); }

			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			bool IsIsolated() const { return halfEdge.expired(); }

			const std::vector<Ptr<HE>> AjdOutHEs();
			const std::vector<PtrC<HE>> AjdOutHEs() const { return Const(const_cast<TVertex*>(this)->AjdOutHEs()); }

			const std::vector<Ptr<E>> AdjEdges();
			const std::vector<PtrC<HE>> AdjEdges() const { return Const(const_cast<TVertex*>(this)->AdjEdges()); }

			const Ptr<HE> FindFreeIncident();
			const PtrC<HE> FindFreeIncident() const { return const_cast<TVertex*>(this)->FindFreeOutHE(); }

			static const Ptr<HE> FindHalfEdge(PtrC<V> v0, PtrC<V> v1);

			const std::vector<Ptr<V>> AdjVertices();
			const std::vector<PtrC<V>> AdjVertices() const { return Const<std::vector, V>(const_cast<TVertex*>(this)->AdjVertices()); }

			static bool IsConnected(PtrC<V> v0, PtrC<V> v1);
			bool IsConnectedWith(PtrC<V> v) const { return IsConnected(This<V>(), v); }
			bool IsBoundary() const;

			size_t Degree() const { return AjdOutHEs().size(); }

		protected:
			virtual ~TVertex() = default;

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

#include <CppUtil/Basic/HEMesh/TVertex.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_VERETX_H_
