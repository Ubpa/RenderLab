#include <Engine/Scene/SObj.h>

#include "SObjSaver.h"
#include "SObjLoader.h"
#include "AssimpLoader.h"

#include <Engine/Scene/Component.h>
#include <Engine/Scene/CmptTransform.h>

#include <UDP/Visitor.h>

#include <Basic/StrAPI.h>

#include <iostream>

using namespace Ubpa;

using namespace std;

void SObj::AttachComponent(Ptr<Component> component) {
	auto target = components.find(typeid(*component));
	if (target != components.end())
		target->second->wSObj.reset();

	component->wSObj = This<SObj>();
	components[typeid(*component)] = component;
}

const std::vector<Ptr<Component>> SObj::GetAllComponents() const {
	std::vector<Ptr<Component>> rst;

	for (auto & component : components)
		rst.push_back(component.second);

	return rst;
}

const transformf SObj::GetLocalToWorldMatrix() {
	auto tsfm = transformf::eye();
	for (auto cur = This<SObj>(); cur != nullptr; cur = cur->GetParent()) {
		auto cmpt = cur->GetComponent<CmptTransform>();
		if (cmpt)
			tsfm = cmpt->GetTransform() * tsfm;
	}
	return tsfm;
}

bool SObj::HaveComponentSameTypeWith(Ptr<Component> ptr) const {
	if (ptr == nullptr)
		return false;

	return components.find(typeid(*ptr)) != components.cend();
}

bool SObj::Save(const string & path) {
	auto saver = SObjSaver::New();
	if (StrAPI::IsEndWith(path, ".sobj"))
		saver->Init(path);
	else
		saver->Init(path + ".sobj");

	saver->Visit(This<SObj>());
	return true;
}

const Ptr<SObj> SObj::Load(const string & path) {
	Ptr<SObj> sobj;
	if (StrAPI::IsEndWith(path, ".sobj"))
		sobj = SObjLoader::Load(path);
	else
		sobj = AssimpLoader::Load(path);

	if (!sobj) {
		printf("WARNING::SObj::Load:\n"
			"\t""loaded sobj is null\n");
	}

	return sobj;
}

bool SObj::DetachComponent(Ptr<Component> component) {
	auto target = components.find(typeid(*component));
	if (target == components.end())
		return false;

	if (target->second != component)
		return false;

	target->second->wSObj.reset();

	components.erase(target);
	return true;
}
