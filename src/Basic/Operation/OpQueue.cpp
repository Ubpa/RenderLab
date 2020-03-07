#include <Basic/Op/OpQueue.h>

#include <stack>

using namespace Ubpa;

using namespace std;

void OpQueue::Push(Ptr<Op> op) {
	if (op != nullptr)
		opList.push_back(op);
}

void OpQueue::Run() {
	stack< list<Ptr<Op>>::const_iterator > removeIts;
	for (auto it = opList.cbegin(); it != opList.cend(); ++it) {
		(*it)->Run();
		if (!(*it)->isHold)
			removeIts.push(it);
	}

	while (!removeIts.empty()) {
		opList.erase(removeIts.top());
		removeIts.pop();
	}

	if (opList.empty())
		isHold = false;
}
