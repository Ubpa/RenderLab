#include <Basic/EventManager.h>

using namespace Ubpa;

#include <Basic/Op/OpQueue.h>
#include <Basic/Op/LambdaOp.h>

void EventMngr::Reg(size_t event, Ptr<Op> op) {
	if (directory.find(event) == directory.end())
		directory[event] = OpQueue::New();
	directory[event]->Push(op);
}

void EventMngr::Reg(size_t event, void * target, Ptr<Op> op) {
	auto tuple2 = std::make_tuple(event, target);
	if (directory20.find(tuple2) == directory20.end())
		directory20[tuple2] = OpQueue::New();

	directory20[tuple2]->Push(op);
}

void EventMngr::Reg(size_t event, ENUM_EVENT_TYPE eventType, Ptr<Op> op) {
	auto tuple2 = std::make_tuple(event, eventType);
	if (directory21.find(tuple2) == directory21.end())
		directory21[tuple2] = OpQueue::New();

	directory21[tuple2]->Push(op);
}

void EventMngr::Reg(size_t event, void * target, ENUM_EVENT_TYPE eventType, Ptr<Op> op) {
	auto tuple3 = std::make_tuple(event, target, eventType);
	if (directory3.find(tuple3) == directory3.end())
		directory3[tuple3] = OpQueue::New();

	directory3[tuple3]->Push(op);
}

void EventMngr::Response(size_t event) {
	auto it = directory.find(event);
	if (it != directory.end())
		it->second->Run();
}

void EventMngr::Response(size_t event, ENUM_EVENT_TYPE eventType) {
	auto tuple2 = std::make_tuple(event, eventType);
	auto it = directory21.find(tuple2);
	if (it != directory21.end())
		it->second->Run();

	Response(event);
}

void EventMngr::Response(size_t event, void * target) {
	auto tuple2 = std::make_tuple( event,target );
	auto it = directory20.find(tuple2);
	if (it != directory20.end())
		it->second->Run();

	Response(event);
}

void EventMngr::Response(size_t event, void * target, ENUM_EVENT_TYPE eventType) {
	auto tuple3 = std::make_tuple( event,target,eventType );
	auto it = directory3.find(tuple3);
	if (it != directory3.end())
		it->second->Run();

	Response(event, target);
	Response(event, eventType);
}
