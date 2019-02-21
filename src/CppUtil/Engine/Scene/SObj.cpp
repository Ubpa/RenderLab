#include <CppUtil/Engine/SObj.h>

#include "SObjSaver.h"
#include "SObjLoader.h"

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Basic/EleVisitor.h>

#include <iostream>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

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

bool SObj::HaveComponentSameTypeWith(Component::Ptr ptr) const {
	if (ptr == nullptr)
		return false;

	return components.find(typeid(*ptr)) != components.cend();
}

bool SObj::Save(const string & path) {
	auto saver = ToPtr(new SObjSaver);
	saver->Init(path);
	Accept(saver);
	return true;
}

SObj::Ptr SObj::Load(const string & path) {
	return SObjLoader::Load(path);
}
