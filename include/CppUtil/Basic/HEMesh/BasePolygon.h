#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_POLYGON_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_POLYGON_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class BaseHalfEdge;

		class BasePolygon : public HeapObj {
		public:
			const Ptr<BaseHalfEdge> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<BaseHalfEdge> he) { halfEdge = he; }

		private:
			WPtr<BaseHalfEdge> halfEdge;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_POLYGON_H_
