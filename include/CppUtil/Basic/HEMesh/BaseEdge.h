#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_EDGE_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_EDGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <tuple>

namespace CppUtil {
	namespace Basic {
		class BaseHalfEdge;

		class BaseEdge : public HeapObj {
		public:
			const Ptr<BaseHalfEdge> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<BaseHalfEdge> he) { halfEdge = he; }

		private:
			WPtr<BaseHalfEdge> halfEdge;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_EDGE_H_
