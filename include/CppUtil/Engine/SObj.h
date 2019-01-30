#ifndef _SOBJ_H_
#define _SOBJ_H_ 

#include <CppUtil/Engine/Component.h>

#include <CppUtil/Basic/LStorage.h>
#include <CppUtil/Basic/Node.h>

#include <string>

namespace CppUtil {
	namespace Engine {

		class SObj : public CppUtil::Basic::Node {
			ELE_SETUP(SObj)
		public:
			template<typename T>
			void AddComponent(Basic::Ptr<T> component);
			template<typename T>
			Basic::Ptr<T> GetComponent(T * useless_parameter = nullptr);

			const std::string & GetName() const { return name; }
			void SetName(const std::string & name) { this->name = name; }
		private:
			std::string name;
			Basic::LStorage<Component::Ptr> components;
		};

		template<typename T>
		Basic::Ptr<T> SObj::GetComponent(T *) {
			Basic::Ptr<T> component(nullptr);
			components.GetV(T::GetClassName(), component);
			return component;
		}

		template<typename T>
		void SObj::AddComponent(Basic::Ptr<T> component) {
			components.Reg(T::GetClassName(), component);
		}
	}
}
#endif
