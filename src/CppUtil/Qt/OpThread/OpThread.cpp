#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Basic/LambdaOp.h>

#include <QMetaType>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;
using namespace std;


OpThread::OpThread(Operation::Ptr op)
	: op(op), isStop(false) {
	qRegisterMetaType<Basic::Ptr<Basic::Operation>>("Basic::Ptr<Basic::Operation>");
}

OpThread::OpThread(function<void()> op)
	: op(ToPtr(new LambdaOp(op))) { }

void OpThread::SetOp(std::function<void()> op) {
	this->op = ToPtr(new LambdaOp(op));
}

void OpThread::run() {
	isStop = false;

	if(op!=nullptr)
		op->Run();

	isStop = true;
}

void OpThread::UI_Op_Run(Operation::Ptr op) {
	emit UI_Op(op);
}

void OpThread::UI_Op_Run(function<void()> op) {
	UI_Op_Run(ToPtr(new LambdaOp(op)));
}