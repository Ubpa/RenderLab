#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
#define _CPPUTIL_BASIC_HEMESH_T_POLYGON_H_

#include <CppUtil/Basic/HEMesh/THalfEdge.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class TPolygon : public HeapObj {
		private:
			using HE = THalfEdge<V, E, P>;

		public:
			const Ptr<THalfEdge<V,E,P>> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<THalfEdge<V, E, P>> he) { halfEdge = he; }

			const std::vector<Ptr<HE>> BoundaryHEs();

		private:
			WPtr<THalfEdge<V, E, P>> halfEdge;
		};

#include <CppUtil/Basic/HEMesh/TPolygon.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_POLYGON_H_
