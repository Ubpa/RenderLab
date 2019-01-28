#include <CppUtil/Basic/EventManager.h>

#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Basic;

EventManager * EventManager::GetInstance() {
	return Singleton<EventManager>::GetInstance();
}

//------------

void EventManager::Reg(size_t event, Ptr<Operation> op) {
	if (directory.find(event) == directory.end())
		directory[event] = ToPtr(new OpQueue);
	directory[event]->Push(op);
}


void EventManager::Reg(size_t event, const std::function<void ()> & op) {
	Reg(event, ToPtr(new LambdaOp(op)));
}

void EventManager::Response(size_t event) {
	auto it = directory.find(event);
	if (it != directory.end())
		it->second->Run();
}