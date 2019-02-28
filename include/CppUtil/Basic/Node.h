#ifndef _BASIC_NODE_NODE_H_
#define _BASIC_NODE_NODE_H_

#include <CppUtil/Basic/Element.h>

#include <set>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Node : public Element {
			ELE_SETUP(Node<T>)

		public:
			Node(Basic::Ptr<T> parent = nullptr)
				: parent(parent) { }

			void AddChild(Basic::Ptr<T> child) {
				if (!child->parent.expired())
					child->parent.lock()->DelChild(child);

				child->parent = Basic::Ptr<T>::Cast(This());
				children.insert(child);
			}

			void DelChild(Basic::Ptr<T> child) {
				if (child->parent.lock() == This()) {
					children.erase(child);
					child->parent.reset();
				}
			}

			void SetParent(Basic::Ptr<T> parent) {
				parent->AddChild(Basic::Ptr<T>::Cast(This()));
			}

			Basic::Ptr<T> GetParent() const { return parent.lock(); }
			const std::set<Basic::Ptr<T>> & GetChildren() const { return children; }

			bool IsAncestorOf(Basic::CPtr<T> node) const {
				return node->IsDescendantOf(Basic::CPtr<T>::Cast(CThis()));
			}

			bool IsDescendantOf(Basic::CPtr<T> node) const {
				if (CThis() == node)
					return true;

				if (parent.expired())
					return false;

				return parent.lock()->IsDescendantOf(node);
			}

		public:
			void TraverseAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor) {
				eleVisitor->Visit(Basic::Ptr<T>::Cast(This()));
				for (auto child : GetChildren())
					child->TraverseAccept(eleVisitor);
			}

			void AscendAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor) {
				eleVisitor->Visit(Basic::Ptr<T>::Cast(This()));
				if (GetParent())
					GetParent()->AscendAccept(eleVisitor);
			}

			virtual void InitAfterGenSharePtr() = 0;

		private:
			Basic::WPtr<T> parent;
			std::set<Basic::Ptr<T>> children;
		};

		template<typename T>
		void Node<T>::InitAfterGenSharePtr() {
			if (GetParent())
				GetParent()->AddChild(Basic::Ptr<T>::Cast(This()));
		}
	}
}

#endif // !_BASIC_NODE_NODE_H_
