#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_EDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_EDGE_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class TEdge : public HeapObj {
		public:
			const Ptr<THalfEdge<V, E, P>> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<THalfEdge<V, E, P> > he) { halfEdge = he; }

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_EDGE_H_
