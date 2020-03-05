#include <Basic/Visitor.h>

#include <Basic/Element.h>

using namespace Ubpa;

void Visitor::Visit(Ptr<Element> ele) {
	// ¶¯Ì¬µ¥·ÖÅÉ
	auto target = visitOps.find(typeid(*ele));
	if (target != visitOps.end())
		target->second(ele);
}
