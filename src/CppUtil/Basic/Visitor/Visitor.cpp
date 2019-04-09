#include <CppUtil/Basic/Visitor.h>

#include <CppUtil/Basic/Element.h>

using namespace CppUtil::Basic;

void Visitor::Visit(Ptr<Element> ele) {
	auto target = visitOps.find(typeid(*ele));
	if (target != visitOps.end())
		target->second(ele);
}
