#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Basic;

LambdaOp::LambdaOp(const std::function<void()> & op, bool isHold)
	: op(op), Operation(isHold) { }

//------------

void LambdaOp::SetOp(const std::function<void()> & op) {
	this->op = op;
}

void LambdaOp::Run() {
	if (op == nullptr) {
		SetIsHold(false);
		return;
	}
	
	op();
}
