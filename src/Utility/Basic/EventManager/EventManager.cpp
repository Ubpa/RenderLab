#include <Utility/Basic/EventManager.h>

#include <Utility/Basic/OpQueue.h>
#include <Utility/Basic/LambdaOp.h>

using namespace CppUtility::Other;

EventManager * EventManager::GetInstance() {
	return Singleton<EventManager>::GetInstance();
}

//------------

void EventManager::Register(size_t event, Ptr<Operation> op) {
	if (directory.find(event) == directory.end())
		directory[event] = ToPtr(new OpQueue);
	directory[event]->Push(op);
}


void EventManager::Register(size_t event, const std::function<void ()> & op) {
	Register(event, ToPtr(new LambdaOp(op)));
}

void EventManager::Response(size_t event) {
	auto it = directory.find(event);
	if (it != directory.end())
		it->second->Run();
}