#ifndef _BASIC_NODE_NODE_H_
#define _BASIC_NODE_NODE_H_

#include <CppUtil/Basic/Element.h>

#include <set>

namespace CppUtil {
	namespace Basic {
		template<typename ImplT>
		class Node : public Element<ImplT> {
		protected:
			Node(Ptr parent = nullptr)
				: parent(parent) { }
		public:
			// 解除 child 的原父子关系，然后再设置新父子关系 
			void AddChild(Ptr child) {
				if (!child->parent.expired())
					child->parent.lock()->DelChild(child);

				child->parent = This();
				children.insert(child);
			}

			void DelChild(Ptr child) {
				if (child->parent.lock() == This()) {
					children.erase(child);
					child->parent.reset();
				}
			}

			const Ptr GetParent() const { return parent.lock(); }
			const std::set<Ptr> & GetChildren() const { return children; }

			bool IsDescendantOf(Ptr node) const {
				if (CThis() == node)
					return true;

				if (parent.expired())
					return false;

				return parent.lock()->IsDescendantOf(node);
			}

			bool IsAncestorOf(Ptr node) const {
				return node->IsDescendantOf(CThis());
			}

		public:
			void TraverseAccept(Basic::Ptr<Visitor::VisitorBase> visitor) {
				visitor->Visit(This());
				for (auto child : GetChildren())
					child->TraverseAccept(visitor);
			}

			void AscendAccept(Basic::Ptr<Visitor::VisitorBase> visitor) {
				visitor->Visit(This());
				const auto parent = GetParent();
				if (parent)
					parent->AscendAccept(visitor);
			}

		protected:
			virtual void Init() override {
				const auto parent = GetParent();
				if (parent)
					parent->AddChild(visitor);
			}

		private:
			WPtr parent;
			std::set<Ptr> children;
		};
	}
}

#endif // !_BASIC_NODE_NODE_H_
