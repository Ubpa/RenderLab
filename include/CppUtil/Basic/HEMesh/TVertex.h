#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_VERETX_H_
#define _CPPUTIL_BASIC_HEMESH_T_VERETX_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <vector>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class TVertex : public HeapObj {
		public:
			using E_t = E;
			using P_t = P;
			using HE = THalfEdge<V, E, P>;

		public:
			const Ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			bool IsIsolated() const { return halfEdge.expired(); }

			const std::vector<Ptr<HE>> AjdOutHEs();
			const std::vector<Ptr<E>> AdjEdges();
			const Ptr<HE> FindFreeOutHE();
			const std::vector<Ptr<V>> AdjVertices();
			static bool IsConnected(Ptr<V> v0, Ptr<V> v1);
			bool IsConnectedWith(Ptr<V> v) { return IsConnected(This<V>(), v); }

		protected:
			virtual ~TVertex() = default;

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

#include <CppUtil/Basic/HEMesh/TVertex.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_VERETX_H_
