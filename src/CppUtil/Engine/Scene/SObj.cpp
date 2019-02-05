#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Basic/EleVisitor.h>

#include <iostream>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

void SObj::AttachComponent(CppUtil::Basic::Ptr<Component> component) {
	auto target = components.find(typeid(*component));
	if (target != components.end())
		target->second->Detach();

	components[typeid(*component)] = component;
}

std::vector<CppUtil::Basic::Ptr<Component>> SObj::GetAllComponents() const {
	std::vector<CppUtil::Basic::Ptr<Component>> rst;

	for (auto & component : components)
		rst.push_back(component.second);

	return rst;
}

mat4 SObj::GetLocalToWorldMatrix() {
	mat4 mat(1.0f);

	auto getMatVisitor = ToPtr(new EleVisitor);
	getMatVisitor->Reg<SObj>([&mat](SObj::Ptr sobj)->bool {
		auto transform = sobj->GetComponent<Transform>();
		if (transform != nullptr)
			mat = transform->GetMat() * mat;

		return true;
	});

	AscendAccept(getMatVisitor);
	return mat;
}
