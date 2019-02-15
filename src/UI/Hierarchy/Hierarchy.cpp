#include <UI/Hierarchy.h>

#include <UI/Attribute.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/EleVisitor.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace Ui;

class Hierarchy::InitHierarchyVisitor : public EleVisitor {
	ELEVISITOR_SETUP(InitHierarchyVisitor)
public:
	InitHierarchyVisitor(Hierarchy * hierarchy)
		: hierarchy(hierarchy) {
		Reg<SObj>();
	}
	
private:
	void Visit(SObj::Ptr sobj) {
		QTreeWidgetItem * item;

		if (sobj == hierarchy->scene->GetRoot())
			item = new QTreeWidgetItem(hierarchy->tree);
		else
			item = new QTreeWidgetItem(hierarchy->sobj2item[sobj->GetParent()]);

		item->setText(0, QString::fromStdString(sobj->name));

		hierarchy->item2sobj[item] = sobj;
		hierarchy->sobj2item[sobj] = item;
	}

private:
	Hierarchy * hierarchy;
};

Hierarchy::Hierarchy()
	: scene(nullptr), tree(nullptr) {
}

void Hierarchy::SetScene(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene) {
	if (!tree)
		return;

	tree->clear();
	this->scene = scene;
	item2sobj.clear();
	item2sobj.clear();

	auto visitor = ToPtr(new InitHierarchyVisitor(this));
	scene->GetRoot()->TraverseAccept(visitor);
}

void Hierarchy::Init(Scene::Ptr scene, QTreeWidget * tree) {
	this->tree = tree;
	SetScene(scene);
}
