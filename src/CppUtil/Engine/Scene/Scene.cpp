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

void Scene::GenID() {
	name2ID.clear();

	auto visitor = ToPtr(new EleVisitor);
	visitor->Reg<SObj>([&](SObj::Ptr sobj) {
		auto target = name2ID.find(sobj->name);
		if (target != name2ID.end())
			printf("WARNING: two sobjs have same name.\n");

		name2ID[sobj->name] = -1;//tmp invalid ID
		for (auto child : sobj->GetChildren())
			child->Accept(visitor);
	});

	root->Accept(visitor);

	int curID = 0;
	for (auto & pair : name2ID) {
		pair.second = curID;
		++curID;
	}
}

int Scene::GetID(SObj::Ptr sobj) const {
	auto target = name2ID.find(sobj->name);
	if (target == name2ID.cend())
		return -1;

	return target->second;
}
