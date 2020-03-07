#include <UI/Hierarchy.h>

#include <UI/Attribute.h>

#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptTransform.h>

#include <UDP/Visitor.h>

#include <qlineedit.h>

using namespace Ubpa;

using namespace std;

Hierarchy::Hierarchy()
	: scene(nullptr), tree(nullptr) {
}

Ptr<SObj> Hierarchy::GetRoot() const {
	return scene->GetRoot();
}

Ptr<SObj> Hierarchy::GetSObj(QTreeWidgetItem * item) const {
	auto target = item2sobj.find(item);
	if (target == item2sobj.cend())
		return nullptr;

	return target->second;
}

QTreeWidgetItem * Hierarchy::GetItem(Ptr<SObj> sobj) const {
	auto target = sobj2item.find(sobj);
	if (target == sobj2item.cend())
		return nullptr;

	return target->second;
}

void Hierarchy::NewItem(QTreeWidget * parent, Ptr<SObj> sobj) {
	auto item = new QTreeWidgetItem(parent);
	item->setText(0, QString::fromStdString(sobj->name));

	item2sobj[item] = sobj;
	sobj2item[sobj] = item;

	for (auto child : sobj->GetChildren())
		NewItem(item, child);
}

void Hierarchy::NewItem(QTreeWidgetItem * parent, Ptr<SObj> sobj) {
	auto item = new QTreeWidgetItem(parent);
	item->setText(0, QString::fromStdString(sobj->name));

	item2sobj[item] = sobj;
	sobj2item[sobj] = item;

	for (auto child : sobj->GetChildren())
		NewItem(item, child);
}

// 自动找一个 item 作为 sobj 的 parent
void Hierarchy::BindSObj(Ptr<SObj> sobj) {
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

Ptr<SObj> Hierarchy::CreateSObj(const std::string & objName) {
	auto item = tree->currentItem();
	if (!item)
		item = sobj2item[scene->GetRoot()];

	auto newSObj = SObj::New(item2sobj[item], objName);
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

	scene->SetWriteLock(true);
	sobj->GetParent()->DelChild(sobj);

	DelItem(item);
	scene->SetWriteLock(false);
}

void Hierarchy::SetScene(Ptr<Scene> scene) {
	if (!tree)
		return;

	tree->clear();
	this->scene = scene;
	item2sobj.clear();
	item2sobj.clear();

	NewItem(tree, scene->GetRoot());
}

void Hierarchy::Init(Ptr<Scene> scene, QTreeWidget * tree) {
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
	auto parentW2L = parentSObj->GetLocalToWorldMatrix().inverse();

	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (!cmptTransform)
		cmptTransform = CmptTransform::New(sobj);

	cmptTransform->SetTransform(parentW2L * sobjL2W);

	parentSObj->AddChild(sobj);
}

void Hierarchy::RenameCurItem() {
	auto item = tree->currentItem();
	if (!item)
		return;

	auto sobj = item2sobj[item];
	auto lineEdit = new QLineEdit;

	lineEdit->setText(QString::fromStdString(sobj->name));
	tree->setItemWidget(item, 0, lineEdit);
	lineEdit->connect(lineEdit, &QLineEdit::editingFinished, [=]() {
		item->setText(0, lineEdit->text());
		sobj->name = lineEdit->text().toStdString();
		tree->removeItemWidget(item, 0);

		delete lineEdit;
	});
}
