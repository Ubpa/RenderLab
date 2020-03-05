#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	template <typename NodeT, typename DataT>
	class KDTree : public HeapObj {
	public:
		KDTree(Ptr<NodeT> parent, int axis, float spiltVal, const DataT& data)
			: parent(parent), axis(axis), spiltVal(spiltVal), data(data) { }

	public:
		int GetAxis() const { return axis; }
		float GetSpiltVal() const { return spiltVal; }
		const DataT& GetData() const { return data; }
		const Ptr<NodeT> GetParent() const { return parent.lock(); }
		const Ptr<NodeT> GetLeft() const { return left; }
		const Ptr<NodeT> GetRight() const { return right; }
		bool IsLeaf() const { return left == nullptr && right == nullptr; }
		bool HasTwoChild() const { return left != nullptr && right != nullptr; }
		bool HasSingleChild() const { return (left == nullptr && right != nullptr) || (left != nullptr && right == nullptr); }

	public:
		bool SetParent(const Ptr<NodeT> parent);
		bool SetLeft(const Ptr<NodeT> child);
		bool SetRight(const Ptr<NodeT> child);

	public:
		Ptr<NodeT> left;
		Ptr<NodeT> right;

	private:
		WPtr<NodeT> parent;

		int axis;
		float spiltVal;
		DataT data;
	};

	template <typename NodeT, typename DataT>
	bool KDTree<NodeT, DataT>::SetParent(const Ptr<NodeT> parent) {
		if (!this->parent.expired())
			return false;

		this->parent = parent;
		return true;
	}

	template <typename NodeT, typename DataT>
	bool KDTree<NodeT, DataT>::SetLeft(const Ptr<NodeT> child) {
		if (child != nullptr && !child->SetParent(This<NodeT>()))
			return false;

		left = child;
		return true;
	}

	template <typename NodeT, typename DataT>
	bool KDTree<NodeT, DataT>::SetRight(const Ptr<NodeT> child) {
		if (child != nullptr && !child->SetParent(This<NodeT>()))
			return false;

		right = child;
		return true;
	}
}
