#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptCamera.h>
#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
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

	bool isFailed = false;
	auto visitor = Visitor::New();
	visitor->Reg([&](Ptr<SObj> sobj) {
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
