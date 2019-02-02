#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Component.h>

#include <iostream>

using namespace CppUtil::Engine;

SObj::~SObj() {
	// 注意避免循环引用

	// 因为 Detach() 会删除 component，所以应该先取到下一个 component
	auto next = components.begin();
	while (next != components.end()) {
		auto cur = next;
		next++;
		cur->second->Detach();
	}
	
}

void SObj::AttachComponent(CppUtil::Basic::Ptr<Component> component) {
	auto target = components.find(typeid(component));
	if (target != components.end())
		target->second->Detach();

	components[typeid(component)] = component;
}

std::vector<CppUtil::Basic::Ptr<Component>> SObj::GetAllComponents() const {
	std::vector<CppUtil::Basic::Ptr<Component>> rst;

	for (auto & component : components)
		rst.push_back(component.second);

	return rst;
}
