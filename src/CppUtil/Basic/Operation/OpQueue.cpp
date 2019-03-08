#include <CppUtil/Basic/OpQueue.h>

#include <stack>

using namespace CppUtil::Basic;
using namespace std;

OpQueue::OpQueue(bool isHold)
	: Operation(isHold) { }

//------------

OpQueue & OpQueue::operator<<(Operation::Ptr op) {
	if (op != nullptr)
		opList.push_back(op);
	return *this;
}

void OpQueue::Push(Operation::Ptr op) {
	if (op != nullptr)
		opList.push_back(op);
}

size_t OpQueue::Size() const {
	return opList.size();
}

void OpQueue::Run() {
	stack< list<Operation::Ptr>::const_iterator > removeIt;
	for (auto it = opList.cbegin(); it != opList.cend(); ++it) {
		(*it)->Run();
		if (!(*it)->IsHold())
			removeIt.push(it);
	}

	while (!removeIt.empty()) {
		opList.erase(removeIt.top());
		removeIt.pop();
	}

	if (opList.empty())
		SetIsHold(false);
}