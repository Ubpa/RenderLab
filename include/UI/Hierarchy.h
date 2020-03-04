#ifndef _UI_HIERARCHY_HIERARCHY_H_
#define _UI_HIERARCHY_HIERARCHY_H_

#include <Basic/HeapObj.h>

#include <QtWidgets/QTreeWidget>

#include <map>

namespace CppUtil {
	namespace Engine {
		class Scene;
		class SObj;
	}
}

namespace Ui {
	class Hierarchy {
	private:
		Hierarchy();

	public:
		static Hierarchy * GetInstance() {
			static Hierarchy * instance = new Hierarchy;
			return instance;
		}

	public:
		void Init(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene, QTreeWidget * tree);
		void SetScene(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene);

		void Move(QTreeWidgetItem * item, QTreeWidgetItem * parent);
		// 这里要求 sobj 已经绑定到场景里了
		void NewItem(QTreeWidget * parent, CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);
		// 这里要求 sobj 已经绑定到场景里了
		void NewItem(QTreeWidgetItem * parent, CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);
		// 自动找一个 item 作为 sobj 的 parent
		void BindSObj(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);
		void DelItem(QTreeWidgetItem * item);
		CppUtil::Basic::Ptr<CppUtil::Engine::SObj> CreateSObj(const std::string & objName);
		void DeleteSObj();

		CppUtil::Basic::Ptr<CppUtil::Engine::SObj> GetSObj(QTreeWidgetItem * item) const;
		CppUtil::Basic::Ptr<CppUtil::Engine::SObj> GetRoot() const;
		QTreeWidgetItem * GetItem(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj) const;

		void RenameCurItem();

	private:
		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
		std::map<CppUtil::Basic::Ptr<CppUtil::Engine::SObj>, QTreeWidgetItem *> sobj2item;
		std::map<QTreeWidgetItem *, CppUtil::Basic::Ptr<CppUtil::Engine::SObj>> item2sobj;
		QTreeWidget * tree;
	};
}

#endif//!_UI_HIERARCHY_HIERARCHY_H_
