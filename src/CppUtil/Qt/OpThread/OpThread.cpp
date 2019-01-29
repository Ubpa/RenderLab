#include <CppUtil/Qt/OpThread.h>
#include <CppUtil/Basic/Operation.h>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;


OpThread::OpThread(Operation::Ptr op)
	: op(op), isStop(false) { }

void OpThread::run() {
	if(op!=nullptr)
		op->Run();
}