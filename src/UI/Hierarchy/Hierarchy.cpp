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

		if (sobj == hierarchy->scene->GetRootSObj()) {
			hierarchy->tree->clear();
			item = new QTreeWidgetItem(hierarchy->tree);
		}
		else
			item = new QTreeWidgetItem(hierarchy->sobj2item[sobj->GetParent()]);

		item->setText(0, QString::fromStdString(sobj->name));

		hierarchy->item2sobj[item] = sobj;
		hierarchy->sobj2item[sobj] = item;
	}

private:
	Hierarchy * hierarchy;
};

Hierarchy::Hierarchy(Scene::Ptr scene, QTreeWidget * tree)
	: scene(scene), tree(tree) {
	Init();
}

void Hierarchy::Init() {
	auto visitor = ToPtr(new InitHierarchyVisitor(this));
	scene->GetRootSObj()->TraverseAccept(visitor);
}
