#include <CppUtil/Engine/SObj.h>

#include "SObjSaver.h"
#include "SObjLoader.h"

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Basic/Visitor.h>

#include <iostream>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

void SObj::AttachComponent(Ptr<ComponentBase> component) {
	auto target = components.find(typeid(*component));
	if (target != components.end())
		target->second->SetSObj(nullptr);

	component->SetSObj(This());
	components[typeid(*component)] = component;
}

const std::vector<Ptr<ComponentBase>> SObj::GetAllComponents() const {
	std::vector<Ptr<ComponentBase>> rst;

	for (auto & component : components)
		rst.push_back(component.second);

	return rst;
}

const Transform SObj::GetLocalToWorldMatrix() {
	Transform tsfm(1.0f);

	auto getMatVisitor = VisitorDynamic::New();
	getMatVisitor->Reg([&](Ptr<SObj> sobj) {
		auto cmpt = sobj->GetComponent<CmptTransform>();
		if (cmpt != nullptr)
			tsfm = cmpt->GetTransform() * tsfm;
	});

	AscendAccept(getMatVisitor);
	return tsfm;
}

bool SObj::HaveComponentSameTypeWith(Ptr<ComponentBase> ptr) const {
	if (ptr == nullptr)
		return false;

	return components.find(typeid(*ptr)) != components.cend();
}

bool SObj::Save(const string & path) {
	auto saver = SObjSaver::New();
	saver->Init(path);
	Accept(saver);
	return true;
}

const Ptr<SObj> SObj::Load(const string & path) {
	return SObjLoader::Load(path);
}
