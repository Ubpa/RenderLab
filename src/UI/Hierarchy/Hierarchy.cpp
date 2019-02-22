#include <UI/Hierarchy.h>

#include <UI/Attribute.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Basic/EleVisitor.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace Ui;
using namespace std;

Hierarchy::Hierarchy()
	: scene(nullptr), tree(nullptr) {
}

SObj::Ptr Hierarchy::GetRoot() const {
	return scene->GetRoot();
}

SObj::Ptr Hierarchy::GetSObj(QTreeWidgetItem * item) const {
	auto target = item2sobj.find(item);
	if (target == item2sobj.cend())
		return nullptr;

	return target->second;
}

QTreeWidgetItem * Hierarchy::GetItem(SObj::Ptr sobj) const {
	auto target = sobj2item.find(sobj);
	if (target == sobj2item.cend())
		return nullptr;

	return target->second;
}

void Hierarchy::NewItem(QTreeWidget * parent, SObj::Ptr sobj) {
	auto item = new QTreeWidgetItem(parent);
	item->setText(0, QString::fromStdString(sobj->name));

	item2sobj[item] = sobj;
	sobj2item[sobj] = item;

	for (auto child : sobj->GetChildren())
		NewItem(item, child);
}

void Hierarchy::NewItem(QTreeWidgetItem * parent, SObj::Ptr sobj) {
	auto item = new QTreeWidgetItem(parent);
	item->setText(0, QString::fromStdString(sobj->name));

	item2sobj[item] = sobj;
	sobj2item[sobj] = item;

	for (auto child : sobj->GetChildren())
		NewItem(item, child);
}

// 自动找一个 item 作为 sobj 的 parent
void Hierarchy::BindSObj(SObj::Ptr sobj) {
	auto item = tree->currentItem();
	if (item == nullptr)
		item = sobj2item[scene->GetRoot()];

	auto parentSObj = item2sobj[item];
	parentSObj->AddChild(sobj);
	NewItem(item, sobj);
}

void Hierarchy::DelItem(QTreeWidgetItem * item) {
	while (item->childCount())
		DelItem(item->child(0));

	auto sobj = item2sobj[item];
	item2sobj.erase(item);
	sobj2item.erase(sobj);
	delete item;
}

SObj::Ptr Hierarchy::CreateSObj(const std::string & objName) {
	auto item = tree->currentItem();
	if (!item)
		item = sobj2item[scene->GetRoot()];

	auto newSObj = ToPtr(new SObj(item2sobj[item], objName));
	NewItem(item, newSObj);

	return newSObj;
}

void Hierarchy::DeleteSObj() {
	auto item = tree->currentItem();
	if (!item)
		return;

	auto sobj = item2sobj[item];
	if (sobj == scene->GetRoot())
		return;

	sobj->GetParent()->DelChild(sobj);

	DelItem(item);
}

void Hierarchy::SetScene(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene) {
	if (!tree)
		return;

	tree->clear();
	this->scene = scene;
	item2sobj.clear();
	item2sobj.clear();

	NewItem(tree, scene->GetRoot());
}

void Hierarchy::Init(Scene::Ptr scene, QTreeWidget * tree) {
	this->tree = tree;
	SetScene(scene);
}

void Hierarchy::Move(QTreeWidgetItem * item, QTreeWidgetItem * parent) {
	auto sobj = GetSObj(item);
	auto parentSObj = GetSObj(parent);
	if (sobj->IsAncestorOf(parentSObj) || sobj->GetParent() == parentSObj)
		return;
	
#ifndef NDEBUG
	printf("%s to %s\n", sobj->name.c_str(), parentSObj->name.c_str());
#endif // !NDEBUG

	auto parentOfItem = sobj2item[sobj->GetParent()];
	parentOfItem->takeChild(parentOfItem->indexOfChild(item));
	parent->addChild(item);

	auto sobjL2W = sobj->GetLocalToWorldMatrix();
	auto parentW2L = parentSObj->GetWorldToLocalMatrix();

	auto transform = sobj->GetComponent<Transform>();
	if (!transform)
		transform = ToPtr(new Transform(sobj));

	transform->SetMatrix(parentW2L * sobjL2W);

	sobj->SetParent(parentSObj);
}
