#include <CppUtil/Basic/Node.h>

using namespace CppUtil::Basic;

void Node::AddChild(Node::Ptr child) {
	if (!child->parent.expired())
		child->parent.lock()->DelChild(child);

	child->parent = This();
	children.insert(child);
}

void Node::DelChild(Node::Ptr child) {
	if (child->parent.lock() == This()) {
		children.erase(child);
		child->parent.reset();
	}
}

void Node::SetParent(Node::Ptr parent) {
	parent->AddChild(This());
}
