#pragma once

#include <Basic/HeapObj.h>

#include <set>

namespace Ubpa {
	template<typename ImplT>
	class Node : public HeapObj {
	protected:
		Node(Ptr<ImplT> parent = nullptr)
			: parent(parent) { }

		virtual ~Node() = default;

	public:
		// 解除 child 的原父子关系，然后再设置新父子关系 
		void AddChild(Ptr<ImplT> child) {
			if (!child->parent.expired())
				child->parent.lock()->DelChild(child);

			child->parent = This<ImplT>();
			children.insert(child);
		}

		void DelChild(Ptr<ImplT> child) {
			if (child->parent.lock() == This()) {
				children.erase(child);
				child->parent.reset();
			}
		}

		const Ptr<ImplT> GetParent() const { return parent.lock(); }
		const std::set<Ptr<ImplT>>& GetChildren() const { return children; }

		bool IsDescendantOf(PtrC<ImplT> node) const {
			if (This<ImplT>() == node)
				return true;

			if (parent.expired())
				return false;

			return parent.lock()->IsDescendantOf(node);
		}

		bool IsAncestorOf(PtrC<ImplT> node) const {
			return node->IsDescendantOf(This<ImplT>());
		}

	public:
		/*void TraverseAccept(Ptr<Visitor> visitor) {
			Accept(visitor);
			for (auto child : GetChildren())
				child->TraverseAccept(visitor);
		}

		void AscendAccept(Ptr<Visitor> visitor) {
			Accept(visitor);
			const auto parent = GetParent();
			if (parent)
				parent->AscendAccept(visitor);
		}*/

	protected:
		virtual void Init_AfterGenPtr() override {
			const auto parent = GetParent();
			if (parent)
				parent->AddChild(This<ImplT>());
		}

	private:
		WPtr<ImplT> parent;
		std::set<Ptr<ImplT>> children;
	};
}
