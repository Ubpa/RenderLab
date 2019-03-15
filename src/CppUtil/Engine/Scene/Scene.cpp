#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Light.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Camera::Ptr Scene::GetCamera() const {
	if (!root)
		return nullptr;

	return root->GetComponentInChildren<Camera>();
}

vector<Light::Ptr> Scene::GetLights() const {
	if (!root)
		return vector<Light::Ptr>();

	return root->GetComponentsInChildren<Light>();
}

bool Scene::GenID() {
	name2ID.clear();
	ID2name.clear();

	bool isFailed = false;
	auto visitor = ToPtr(new EleVisitor);
	visitor->Reg<SObj>([&](SObj::Ptr sobj) {
		auto target = name2ID.find(sobj->name);
		if (target != name2ID.end()) {
			printf("ERROR: two sobjs have same name.\n");
			isFailed = true;
			return;
		}

		name2ID[sobj->name] = 0;//tmp invalid ID
		for (auto child : sobj->GetChildren()) {
			child->Accept(visitor);
			if (isFailed)
				return;
		}
	});

	root->Accept(visitor);

	if (isFailed) {
		name2ID.clear();
		ID2name.clear();
		return false;
	}

	int curID = 1;
	for (auto & pair : name2ID) {
		pair.second = curID;
		++curID;
	}

	for (auto & pair : name2ID)
		ID2name[pair.second] = pair.first;

	return true;
}

int Scene::GetID(SObj::Ptr sobj) const {
	auto target = name2ID.find(sobj->name);
	if (target == name2ID.cend())
		return 0;

	return target->second;
}

string Scene::GetName(int ID) const {
	auto target = ID2name.find(ID);
	if (target == ID2name.cend())
		return "";

	return target->second;
}
