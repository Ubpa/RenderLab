#include <Basic/Element.h>

#include <Basic/Visitor.h>

using namespace CppUtil::Basic;

void Element::Accept(Ptr<Visitor> visitor) {
	visitor->Visit(This<Element>());
}
