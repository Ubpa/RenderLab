#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HALFEDGE_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HALFEDGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <assert.h>

namespace CppUtil {
	namespace Basic {
		class BaseVertex;
		class BaseEdge;
		class BasePolygon;

		class BaseHalfEdge : public HeapObj {
		public:
			const Ptr<BaseHalfEdge> Next() { return next.lock(); }
			const Ptr<BaseHalfEdge> Pair() { return pair.lock(); }
			const Ptr<BaseVertex> Origin() { return origin.lock(); }
			const Ptr<BaseEdge> Edge() { return edge.lock(); }
			const Ptr<BasePolygon> Polygon() { return polygon.lock(); }
			const Ptr<BaseVertex> End() { return pair.lock()->Origin(); }

			void SetNext(Ptr<BaseHalfEdge> he) { next = he; }
			void SetPair(Ptr<BaseHalfEdge> he) { pair = he; }
			void SetOrigin(Ptr<BaseVertex> v) { origin = v; }
			void SetEdge(Ptr<BaseEdge> e) { edge = e; }
			void SetPolygon(Ptr<BasePolygon> p) { polygon = p; }

			bool IsFree() const { return polygon.expired(); }
			const Ptr<BaseHalfEdge> Pre() {
				Ptr<BaseHalfEdge> he;
				for (he = This<BaseHalfEdge>(); he->Next()->Next() != This<BaseHalfEdge>(); he = he->Next())
					;// empty
				return he->Next();
			}
			const Ptr<BaseHalfEdge> RotateNext() { return Pair()->Next(); }

		private:
			WPtr<BaseHalfEdge> next;
			WPtr<BaseHalfEdge> pair;

			WPtr<BaseVertex> origin;
			WPtr<BaseEdge> edge;
			WPtr<BasePolygon> polygon;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HALFEDGE_H_
