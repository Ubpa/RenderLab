#include <Engine/Scene/Scene.h>

#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptCamera.h>
#include <Engine/Scene/CmptLight.h>
#include <Engine/Light/InfiniteAreaLight.h>

#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

void Scene::SetWriteLock(bool isLock) {
	if (isLock)
		writeLock.lock();
	else
		writeLock.unlock();
}

const Ptr<CmptCamera> Scene::GetCmptCamera() const {
	if (!root)
		return nullptr;

	return root->GetComponentInChildren<CmptCamera>();
}

const vector<Ptr<CmptLight>> Scene::GetCmptLights() const {
	if (!root)
		return vector<Ptr<CmptLight>>();

	return root->GetComponentsInChildren<CmptLight>();
}

bool Scene::GenID() {
	name2ID.clear();
	ID2name.clear();

	std::function<bool(Ptr<SObj>)> genID;

	genID = [this, &genID](Ptr<SObj> sobj)->bool {
		auto target = name2ID.find(sobj->name);
		if (target != name2ID.end()) {
			printf("ERROR: two sobjs have same name.\n");
			return false;
		}
		name2ID[sobj->name] = 0;//tmp invalid ID
		for (auto child : sobj->GetChildren()) {
			if (!genID(child))
				return false;
		}
		return true;
	};

	if (!genID(root)) {
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

int Scene::GetID(Ptr<SObj> sobj) const {
	auto target = name2ID.find(sobj->name);
	if (target == name2ID.cend())
		return 0;

	return target->second;
}

const string Scene::GetName(int ID) const {
	auto target = ID2name.find(ID);
	if (target == ID2name.cend())
		return "";

	return target->second;
}

const Ptr<InfiniteAreaLight> Scene::GetInfiniteAreaLight() const {
	auto lightCmpts = GetCmptLights();
	for (auto lightCmpt : lightCmpts) {
		auto rst = CastTo<InfiniteAreaLight>(lightCmpt->light);
		if (rst)
			return rst;
	}

	return nullptr;
}
