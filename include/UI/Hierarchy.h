#ifndef _UI_HIERARCHY_H_
#define _UI_HIERARCHY_H_

#include <CppUtil/Basic/HeapObj.h>

#include <QtWidgets/QTreeWidget>

#include <map>

namespace CppUtil {
	namespace Engine {
		class Scene;
		class SObj;
	}
}

namespace Ui {
	class Hierarchy : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Hierarchy)
	public:
		Hierarchy(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene, QTreeWidget * tree);

		void Init();

	private:
		class InitHierarchyVisitor;
		friend class InitHierarchyVisitor;

		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
		std::map<CppUtil::Basic::Ptr<CppUtil::Engine::SObj>, QTreeWidgetItem *> sobj2item;
		std::map<QTreeWidgetItem *, CppUtil::Basic::Ptr<CppUtil::Engine::SObj>> item2sobj;
		QTreeWidget * tree;
	};
}

#endif//!_UI_HIERARCHY_H_
