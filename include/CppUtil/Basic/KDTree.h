#ifndef _CPPUTIL_BASIC_HEADER_KDTREE_H_
#define _CPPUTIL_BASIC_HEADER_KDTREE_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		template <typename NodeT, typename DataT>
		class KDTree : public Basic::HeapObj {
			HEAP_OBJ_SETUP(KDTree)
		public:
			KDTree(Basic::Ptr<NodeT> parent, int axis, float spiltVal, const DataT & data)
				: parent(parent), axis(axis), spiltVal(spiltVal), data(data) { }

		public:
			const int GetAxis() const { return axis; }
			const float GetSpiltVal() const { return spiltVal; }
			const DataT & GetData() const { return data; }
			const Basic::Ptr<NodeT> GetParent() const { return parent.lock(); }
			const Basic::Ptr<NodeT> GetLeft() const { return left; }
			const Basic::Ptr<NodeT> GetRight() const { return right; }
			const bool IsLeaf() const { return left == nullptr && right == nullptr; }

		public:
			bool SetParent(const Basic::Ptr<NodeT> parent);
			bool SetLeft(const Basic::Ptr<NodeT> child);
			bool SetRight(const Basic::Ptr<NodeT> child);

		public:
			Basic::Ptr<NodeT> left;
			Basic::Ptr<NodeT> right;

		private:
			Basic::WPtr<NodeT> parent;

			int axis;
			float spiltVal;
			DataT data;
		};

		template <typename NodeT, typename DataT>
		bool KDTree<NodeT, DataT>::SetParent(const Basic::Ptr<NodeT> parent) {
			if (!this->parent.expired())
				return false;

			this->parent = parent;
			return true;
		}

		template <typename NodeT, typename DataT>
		bool KDTree<NodeT, DataT>::SetLeft(const Basic::Ptr<NodeT> child) {
			if (!child->SetParent(Basic::Ptr<NodeT>::Cast(This())))
				return false;

			left = child;
			return true;
		}

		template <typename NodeT, typename DataT>
		bool KDTree<NodeT, DataT>::SetRight(const Basic::Ptr<NodeT> child) {
			if (!child->SetParent(Basic::Ptr<NodeT>::Cast(This())))
				return false;

			right = child;
			return true;
		}
	}
}

#endif//!_CPPUTIL_BASIC_HEADER_KDTREE_H_
