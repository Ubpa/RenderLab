#include <CppUtil/Basic/EventManager.h>

#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Basic;

EventMngr * EventMngr::GetInstance() {
	return Singleton<EventMngr>::GetInstance();
}

//------------

void EventMngr::_Reg(size_t event, Operation::Ptr op) {
	if (directory.find(event) == directory.end())
		directory[event] = ToPtr(new OpQueue);
	directory[event]->Push(op);
}

void EventMngr::_Reg(size_t event, void * target, Operation::Ptr op) {
	auto tuple2 = std::make_tuple(event, target);
	if (directory20.find(tuple2) == directory20.end())
		directory20[tuple2] = ToPtr(new OpQueue);

	directory20[tuple2]->Push(op);
}

void EventMngr::_Reg(size_t event, ENUM_EVENT_TYPE eventType, Ptr<Operation> op) {
	auto tuple2 = std::make_tuple(event, eventType);
	if (directory21.find(tuple2) == directory21.end())
		directory21[tuple2] = ToPtr(new OpQueue);

	directory21[tuple2]->Push(op);
}

void EventMngr::_Reg(size_t event, void * target, ENUM_EVENT_TYPE eventType, Operation::Ptr op) {
	auto tuple3 = std::make_tuple(event, target, eventType);
	if (directory3.find(tuple3) == directory3.end())
		directory3[tuple3] = ToPtr(new OpQueue);

	directory3[tuple3]->Push(op);
}

void EventMngr::_Response(size_t event) {
	auto it = directory.find(event);
	if (it != directory.end())
		it->second->Run();
}

void EventMngr::_Response(size_t event, ENUM_EVENT_TYPE eventType) {
	auto tuple2 = std::make_tuple(event, eventType);
	auto it = directory21.find(tuple2);
	if (it != directory21.end())
		it->second->Run();

	_Response(event);
}

void EventMngr::_Response(size_t event, void * target) {
	auto tuple2 = std::make_tuple( event,target );
	auto it = directory20.find(tuple2);
	if (it != directory20.end())
		it->second->Run();

	_Response(event);
}

void EventMngr::_Response(size_t event, void * target, ENUM_EVENT_TYPE eventType) {
	auto tuple3 = std::make_tuple( event,target,eventType );
	auto it = directory3.find(tuple3);
	if (it != directory3.end())
		it->second->Run();

	_Response(event, target);
	_Response(event, eventType);
}
