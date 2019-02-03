#ifndef _NODE_H_
#define _NODE_H_

#include <CppUtil/Basic/Element.h>

#include <set>

#define NODE_SETUP(CLASS) \
ELE_SETUP(CLASS)\
public:\
virtual bool TraverseAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor){\
	if(!eleVisitor->Visit(This()))\
		return false;\
	for (auto child : GetChildren()) {\
		if(!child->TraverseAccept(eleVisitor)) \
			return false;\
	}\
	return true;\
}\
virtual void AscendAccept(CppUtil::Basic::EleVisitor::Ptr eleVisitor){\
	if(!eleVisitor->Visit(This()))\
		return;\
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

		private:
			Node::WPtr parent;
			std::set<Node::Ptr> children;
		};
	}
}

#endif // !_NODE_H_
