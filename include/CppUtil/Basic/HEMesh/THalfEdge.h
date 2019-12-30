#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_
#define _CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <assert.h>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P>
		class THalfEdge : public HeapObj {
		public:
			const Ptr<THalfEdge> Next() { return next.lock(); }
			const Ptr<THalfEdge> Pair() { return pair.lock(); }
			const Ptr<V> Origin() { return origin.lock(); }
			const Ptr<E> Edge() { return edge.lock(); }
			const Ptr<P> Polygon() { return polygon.lock(); }

			const PtrC<THalfEdge> Next() const { return const_cast<THalfEdge*>(this)->Next(); }
			const PtrC<THalfEdge> Pair() const { return const_cast<THalfEdge*>(this)->Pair(); }
			const PtrC<V> Origin() const { return const_cast<THalfEdge*>(this)->Origin(); }
			const PtrC<E> Edge() const { return const_cast<THalfEdge*>(this)->Edge(); }
			const PtrC<P> Polygon() const { return const_cast<THalfEdge*>(this)->Polygon(); }

			const Ptr<V> End() { return pair.lock()->Origin(); }
			const PtrC<V> End() const { return const_cast<THalfEdge*>(this)->End(); }

			void SetNext(Ptr<THalfEdge> he) { next = he; }
			void SetPair(Ptr<THalfEdge> he) { pair = he; }
			void SetOrigin(Ptr<V> v) { origin = v; }
			void SetEdge(Ptr<E> e) { edge = e; }
			void SetPolygon(Ptr<P> p) { polygon = p; }

			bool IsFree() const { return polygon.expired(); }
			const Ptr<THalfEdge> Pre();
			const PtrC<THalfEdge> Pre() const { return const_cast<THalfEdge*>(this)->Pre(); }
			const Ptr<THalfEdge> RotateNext() { return Pair()->Next(); }
			const PtrC<THalfEdge> RotateNext() const { return const_cast<THalfEdge*>(this)->RotateNext(); }

		private:
			WPtr<THalfEdge> next;
			WPtr<THalfEdge> pair;

			WPtr<V> origin;
			WPtr<E> edge;
			WPtr<P> polygon;
		};

#include <CppUtil/Basic/HEMesh/THalfEdge.inl>
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_
