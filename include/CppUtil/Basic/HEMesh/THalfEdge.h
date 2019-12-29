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
			const Ptr<V> End() { return pair.lock()->Origin(); }

			void SetNext(Ptr<THalfEdge> he) { next = he; }
			void SetPair(Ptr<THalfEdge> he) { pair = he; }
			void SetOrigin(Ptr<V> v) { origin = v; }
			void SetEdge(Ptr<E> e) { edge = e; }
			void SetPolygon(Ptr<P> p) { polygon = p; }

			bool IsFree() const { return polygon.expired(); }
			const Ptr<THalfEdge> Pre() {
				Ptr<THalfEdge> he;
				for (he = This<THalfEdge>(); he->Next()->Next() != This<THalfEdge>(); he = he->Next())
					;// empty
				return he->Next();
			}
			const Ptr<THalfEdge> RotateNext() { return Pair()->Next(); }

		private:
			WPtr<THalfEdge> next;
			WPtr<THalfEdge> pair;

			WPtr<V> origin;
			WPtr<E> edge;
			WPtr<P> polygon;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_T_HALFEDGE_H_
