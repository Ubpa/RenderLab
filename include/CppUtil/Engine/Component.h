#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <CppUtil/Basic/Element.h>
#include <functional>

#define COMPONENT_SETUP(CLASS) \
ELE_SETUP_SELF_DEL(CLASS) \
virtual void InitAfterGenSharePtr() { AttachSObj(GetSObj()); } 

namespace CppUtil {
	namespace Engine {
		class SObj;

		class Component : public Basic::Element {
			ELE_SETUP(Component)
		public:
			Component(Basic::Ptr<SObj> sobj);

			void Detach();
			void AttachSObj(Basic::Ptr<SObj> sobj);

			Basic::Ptr<SObj> GetSObj() const { return wSObj.lock(); }
			bool IsEnabled() const { return isEnabled; }
			void SetEnable(bool isEnabled) { this->isEnabled = isEnabled; }

		private:
			Basic::WPtr<SObj> wSObj;
			bool isEnabled;
		};
	}
}

#endif // !_COMPONENT_H_
