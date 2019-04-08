#ifndef _ENGINE_COMPONENT_COMPONENT_H_
#define _ENGINE_COMPONENT_COMPONENT_H_

#include <CppUtil/Basic/Element.h>
#include <CppUtil/Engine/SObj.h>

namespace CppUtil {
	namespace Engine {
		class SObj;

		class ComponentBase : public Basic::ElementBase {
		protected:
			ComponentBase(Basic::Ptr<SObj> sobj) : wSObj(sobj) { }
			virtual ~ComponentBase() = default;

		public:
			const Basic::Ptr<SObj> GetSObj() const { return wSObj.lock(); }
			void SetSObj(Basic::Ptr<SObj> sobj) { wSObj = sobj; }

		private:
			Basic::WPtr<SObj> wSObj;
		};

		template<typename ImplT, typename BaseT = ComponentBase>
		class Component : public Basic::Element<ImplT, BaseT> {
		protected:
			Component(Basic::Ptr<SObj> sobj) : Element<ImplT, BaseT>(sobj) { }
			virtual ~Component() = default;

		protected:
			virtual void Init() override {
				auto sobj = GetSObj();
				if (sobj)
					sobj->AttachComponent(This());
			}
		};
	}
}

#endif // !_ENGINE_COMPONENT_COMPONENT_H_
