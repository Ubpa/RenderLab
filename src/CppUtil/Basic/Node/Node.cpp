#include <CppUtil/Basic/Node.h>

using namespace CppUtil::Basic;

void Node::AddChild(Node::Ptr child) {
	children.push_back(child);
}
