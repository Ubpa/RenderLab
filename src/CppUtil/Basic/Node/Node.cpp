#include <CppUtil/Basic/Node.h>

using namespace CppUtil::Basic;

void Node::AddChild(Node::Ptr child) {
	children.push_back(child);
	child->AddParent(This());
}

void Node::AddParent(Node::Ptr parent) {
	parents.push_back(parent);
}
