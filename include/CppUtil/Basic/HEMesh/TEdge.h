#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_EDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_EDGE_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge;

		template<typename V, typename E, typename P>
		class TEdge : public HeapObj {
		public:
			const Ptr<THalfEdge<V, E, P>> HalfEdge() { return halfEdge.lock(); }
			const Ptr<THalfEdge<V, E, P>> HalfEdge() const { return const_cast<TEdge*>(this)->HalfEdge(); }
			
			bool IsBoundary() const { return HalfEdge()->IsBoundary() || HalfEdge()->Pair()->IsBoundary(); }

			void SetHalfEdge(Ptr<THalfEdge<V, E, P> > he) { halfEdge = he; }

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_EDGE_H_
