#include <CppUtil/Qt/OpThread.h>
#include <CppUtil/Basic/Op.h>
#include <QMetaType>

using namespace CppUtil::QT;
using namespace CppUtil::Basic;
using namespace std;

OpThread::OpThread(CppUtil::Basic::Ptr<Op> op)
	: op(op), isStop(false) {
	qRegisterMetaType<Basic::Ptr<Basic::Op>>("Basic::Ptr<Basic::Op>");
}

void OpThread::run() {
	isStop = false;

	if(op!=nullptr)
		op->Run();

	isStop = true;
}

void OpThread::UI_Op_Run(CppUtil::Basic::Ptr<Op> op) {
	emit UI_Op(op);
}
