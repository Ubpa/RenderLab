#ifndef _UI_ATTRIBUTE_H_
#define _UI_ATTRIBUTE_H_

#include <CppUtil/Basic/HeapObj.h>

#include <qobject.h>
#include <qtoolbox.h>

namespace CppUtil {
	namespace Engine {
		class SObj;
		class Component;
	}
}


namespace Ui {
	class Attribute : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(Attribute)
	public:
		Attribute(QToolBox * tbox);

		void SetSObj(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

	private:
		class ComponentVisitor;
		friend class ComponentVisitor;

		QToolBox * tbox;
		CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj;

		std::map<CppUtil::Basic::Ptr<CppUtil::Engine::Component>, QWidget *> component2item;
		std::map<QWidget *, CppUtil::Basic::Ptr<CppUtil::Engine::Component>> item2component;

		CppUtil::Basic::Ptr<ComponentVisitor> visitor;
	};
}

#endif//!_UI_ATTRIBUTE_H_
