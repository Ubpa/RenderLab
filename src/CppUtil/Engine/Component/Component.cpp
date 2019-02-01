#include <CppUtil/Engine/Component.h>

using namespace CppUtil;
using namespace CppUtil::Engine;

Component::Component(Basic::Ptr<SObj> sobj)
	: wSObj(sobj), isEnabled(true), detachOp(nullptr) { }

void Component::Detach() {
	if (detachOp != nullptr) {
		detachOp();
		detachOp = nullptr;
	}
}
