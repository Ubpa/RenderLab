#pragma once

#include <Basic/HeapObj.h>

#include <QtWidgets/QTreeWidget>

#include <map>

namespace Ubpa {
	class Scene;
	class SObj;

	class Hierarchy {
	private:
		Hierarchy();

	public:
		static Hierarchy * GetInstance() {
			static Hierarchy * instance = new Hierarchy;
			return instance;
		}

	public:
		void Init(Ptr<Scene> scene, QTreeWidget * tree);
		void SetScene(Ptr<Scene> scene);

		void Move(QTreeWidgetItem * item, QTreeWidgetItem * parent);
		// 这里要求 sobj 已经绑定到场景里了
		void NewItem(QTreeWidget * parent, Ptr<SObj> sobj);
		// 这里要求 sobj 已经绑定到场景里了
		void NewItem(QTreeWidgetItem * parent, Ptr<SObj> sobj);
		// 自动找一个 item 作为 sobj 的 parent
		void BindSObj(Ptr<SObj> sobj);
		void DelItem(QTreeWidgetItem * item);
		Ptr<SObj> CreateSObj(const std::string & objName);
		void DeleteSObj();

		Ptr<SObj> GetSObj(QTreeWidgetItem * item) const;
		Ptr<SObj> GetRoot() const;
		QTreeWidgetItem * GetItem(Ptr<SObj> sobj) const;

		void RenameCurItem();

	private:
		Ptr<Scene> scene;
		std::map<Ptr<SObj>, QTreeWidgetItem *> sobj2item;
		std::map<QTreeWidgetItem *, Ptr<SObj>> item2sobj;
		QTreeWidget * tree;
	};
}
