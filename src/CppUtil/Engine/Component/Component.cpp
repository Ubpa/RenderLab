#include <CppUtil/Engine/Component.h>

#include <CppUtil/Engine/SObj.h>

using namespace CppUtil;
using namespace CppUtil::Engine;

Component::Component(Basic::Ptr<SObj> sobj)
	: wSObj(sobj), isEnabled(true) { }

void Component::Detach() {
	if (!wSObj.expired()) {
		wSObj.lock()->DetachComponent(This());
		wSObj.reset();
	}
}

void Component::AttachSObj(Basic::Ptr<SObj> sobj) {
	if(sobj != nullptr)
		sobj->AttachComponent(This());
	wSObj = sobj;
}
