#ifndef _BASIC_NODE_NODE_H_
#define _BASIC_NODE_NODE_H_

#include <CppUtil/Basic/Element.h>

#include <set>

#define NODE_SETUP(CLASS)\
ELE_SETUP(CLASS)\
public:\
virtual void TraverseAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor) {\
	eleVisitor->Visit(This());\
	for (auto child : GetChildren())\
		child->TraverseAccept(eleVisitor);\
}\
virtual void AscendAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor) {\
	eleVisitor->Visit(This());\
	if(GetParent())\
		GetParent()->AscendAccept(eleVisitor);\
}\
virtual void InitAfterGenSharePtr() {\
	if(GetParent())\
		GetParent()->AddChild(This());\
} 

namespace CppUtil {
	namespace Basic {
		class Node : public Element {
			NODE_SETUP(Node)

		public:
			Node(Node::Ptr parent = nullptr)
				: parent(parent) { }

			void AddChild(Node::Ptr child);
			void DelChild(Node::Ptr child);
			void SetParent(Node::Ptr parent);
			Node::Ptr GetParent() const { return parent.lock(); }
			const std::set<Node::Ptr> & GetChildren() const { return children; }

			bool IsAncestorOf(Node::CPtr node) const;
			bool IsDescendantOf(Node::CPtr node) const;

		private:
			Node::WPtr parent;
			std::set<Node::Ptr> children;
		};
	}
}

#endif // !_BASIC_NODE_NODE_H_
