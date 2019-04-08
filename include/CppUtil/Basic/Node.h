#ifndef _BASIC_NODE_NODE_H_
#define _BASIC_NODE_NODE_H_

#include <CppUtil/Basic/Element.h>

#include <set>

namespace CppUtil {
	namespace Basic {
		template<typename ImplT, typename BaseT = ElementBase>
		class Node : public Element<ImplT, BaseT> {
		protected:
			using Element<ImplT, BaseT>::Element;

			Node(Ptr<ImplT> parent = nullptr)
				: parent(parent) { }

		public:
			// 解除 child 的原父子关系，然后再设置新父子关系 
			void AddChild(Ptr<ImplT> child) {
				if (!child->parent.expired())
					child->parent.lock()->DelChild(child);

				child->parent = This();
				children.insert(child);
			}

			void DelChild(Ptr<ImplT> child) {
				if (child->parent.lock() == This()) {
					children.erase(child);
					child->parent.reset();
				}
			}

			const Ptr<ImplT> GetParent() const { return parent.lock(); }
			const std::set<Ptr<ImplT>> & GetChildren() const { return children; }

			bool IsDescendantOf(Ptr<ImplT> node) const {
				if (This() == node)
					return true;

				if (parent.expired())
					return false;

				return parent.lock()->IsDescendantOf(node);
			}

			bool IsAncestorOf(Ptr<ImplT> node) const {
				return node->IsDescendantOf(This());
			}

		public:
			void TraverseAccept(Ptr<VisitorBase> visitor) {
				Element<ImplT, BaseT>::Accept(visitor);
				for (auto child : GetChildren())
					child->TraverseAccept(visitor);
			}

			void AscendAccept(Ptr<VisitorBase> visitor) {
				Element<ImplT, BaseT>::Accept(visitor);
				const auto parent = GetParent();
				if (parent)
					parent->AscendAccept(visitor);
			}

		protected:
			virtual void Init() override {
				const auto parent = GetParent();
				if (parent)
					parent->AddChild(This());
			}

		private:
			WPtr<ImplT> parent;
			std::set<Ptr<ImplT>> children;
		};
	}
}

#endif // !_BASIC_NODE_NODE_H_
