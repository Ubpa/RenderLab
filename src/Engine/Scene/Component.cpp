#include <Engine/Component.h>

#include <Engine/SObj.h>

using namespace CppUtil::Engine;

void Component::Init_AfterGenPtr() {
	auto sobj = GetSObj();
	if (sobj)
		sobj->AttachComponent(This<Component>());
}
