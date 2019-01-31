#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Component : public Basic::Element {
			ELE_SETUP(Component)
		public:
			Component(Basic::Ptr<SObj> sobj = nullptr)
				: sobj(sobj) { }

			Basic::Ptr<SObj> GetSObj() { return sobj; }
			bool IsEnabled() const { return isEnabled; }
			void SetEnable(bool isEnabled) { this->isEnabled = isEnabled; }
		private:
			Basic::Ptr<SObj> sobj;
			bool isEnabled;
		};
	}
}

#endif // !_COMPONENT_H_
