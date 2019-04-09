#include <CppUtil/Engine/Component.h>

#include <CppUtil/Engine/SObj.h>

using namespace CppUtil::Engine;

void Component::Init() {
	auto sobj = GetSObj();
	if (sobj)
		sobj->AttachComponent(This<Component>());
}
