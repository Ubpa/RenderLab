#include <CppUtil/Basic/OpNode.h>

using namespace CppUtil::Basic;

OpNode::OpNode(Operation::Ptr preOp, Operation::Ptr postOp, bool isHold)
	: preOp(preOp), postOp(postOp), OpQueue(isHold) { }

OpNode::OpNode(const std::function<void()> & preOp, const std::function<void()> & postOp, bool isHold)
	: OpQueue(isHold){
	this->preOp = preOp != nullptr ? ToPtr(new LambdaOp(preOp)) : nullptr;
	this->postOp = postOp != nullptr ? ToPtr(new LambdaOp(postOp)) : nullptr;
}

//------------
void OpNode::SetPreOp(Operation::Ptr preOp) {
	this->preOp = preOp;
}

void OpNode::SetPostOp(Operation::Ptr postOp) {
	this->postOp = postOp;
}

size_t OpNode::ChildNum() const{
	return Size();
}

void OpNode::Run() {
	if (preOp != nullptr) {
		preOp->Run();
		if (!preOp->IsHold())
			preOp = nullptr;
	}

	OpQueue::Run();

	if (postOp != nullptr) {
		postOp->Run();
		if (!postOp->IsHold())
			postOp = nullptr;
	}

	if (Size() == 0 && preOp == nullptr && postOp == nullptr)
		SetIsHold(false);
}