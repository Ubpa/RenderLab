#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_VERETX_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_VERETX_H_

#include <CppUtil/Basic/HeapObj.h>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class BaseHalfEdge;

		class BaseVertex : public HeapObj {
		public:
			const Ptr<BaseHalfEdge> HalfEdge() { return halfEdge.lock(); }
			void SetHalfEdge(Ptr<BaseHalfEdge> he) { halfEdge = he; }
			bool IsIsolated() const { return halfEdge.expired(); }

		protected:
			virtual ~BaseVertex() = default;

		private:
			WPtr<BaseHalfEdge> halfEdge;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_VERETX_H_
