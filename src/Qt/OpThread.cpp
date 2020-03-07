#include <Qt/OpThread.h>
#include <Basic/Op/Op.h>
#include <QMetaType>

using namespace Ubpa;

using namespace std;

OpThread::OpThread(Ptr<Op> op)
	: op(op), isStop(false) {
	qRegisterMetaType<Ptr<Op>>("Ptr<Op>");
}

void OpThread::run() {
	isStop = false;

	if(op!=nullptr)
		op->Run();

	isStop = true;
}

void OpThread::UI_Op_Run(Ptr<Op> op) {
	emit UI_Op(op);
}
