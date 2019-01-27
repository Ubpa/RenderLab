#include <CppUtil/Basic/OpQueue.h>

#include <vector>

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
	vector< list<Operation::Ptr>::const_iterator > removeIt;
	for (auto it = opList.cbegin(); it != opList.cend(); ++it) {
		(*it)->Run();
		if (!(*it)->IsHold())
			removeIt.push_back(it);
	}
	for (int i = removeIt.size() - 1; i >= 0; i--)
		opList.erase(removeIt[i]);

	if (opList.empty())
		SetIsHold(false);
}