#include <Engine/Scene/Component.h>

#include <Engine/Scene/SObj.h>

using namespace Ubpa;

void Component::Init_AfterGenPtr() {
	auto sobj = GetSObj();
	if (sobj)
		sobj->AttachComponent(This<Component>());
}
