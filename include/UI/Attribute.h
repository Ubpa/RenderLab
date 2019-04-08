#ifndef _UI_ATTRIBUTE_H_
#define _UI_ATTRIBUTE_H_

#include <CppUtil/Basic/TypeMap.h>

#include <CppUtil/Basic/Ptr.h>

#include <qobject.h>
#include <qtoolbox.h>

#include <map>

namespace CppUtil {
	namespace Engine {
		class SObj;
		class ComponentBase;
	}
}


namespace Ui {
	class Grid;

	class Attribute final {
	protected:
		Attribute();

	public:
		static Attribute * GetInstance() {
			static Attribute instance;
			return &instance;
		}

	public:
		void Init(QToolBox * tbox);
		void SetSObj(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

	private:
		void AddController(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

	private:
		class ComponentVisitor;
		friend class ComponentVisitor;

		QToolBox * tbox;

		CppUtil::Basic::TypeMap<QWidget *> componentType2item;
		std::map<QWidget *, CppUtil::Basic::Ptr<Grid>> item2grid;

		CppUtil::Basic::Ptr<ComponentVisitor> visitor;
	};
}

#endif//!_UI_ATTRIBUTE_H_
