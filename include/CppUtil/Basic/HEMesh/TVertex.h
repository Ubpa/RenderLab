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
			const Ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }
			bool IsIsolated() const { return halfEdge.expired(); }

		protected:
			virtual ~TVertex() = default;

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_VERETX_H_
