#ifndef _SOBJ_H_
#define _SOBJ_H_ 

#include <CppUtil/Basic/LStorage.h>
#include <CppUtil/Basic/Node.h>

#include <string>
#include <set>

namespace CppUtil {
	namespace Engine {
		class Component;

		class SObj : public CppUtil::Basic::Node {
			ELE_SETUP_SELF_DEL(SObj)
		public:
			SObj(const std::string & name = "")
				: name(name) { }

			template<typename T>
			void AttachComponent(Basic::Ptr<T> component);
			template<typename T>
			Basic::Ptr<T> GetComponent(T * useless_parameter = nullptr);
			template<typename T>
			void DetachComponent(T * useless_parameter = nullptr);

			const std::string & GetName() const { return name; }
			void SetName(const std::string & name) { this->name = name; }

			const std::set<Basic::Ptr<Component>> & GetAllComponents() const { return components; }

		protected:
			~SObj();

		private:
			enum MODE { ATTACH, GET, DETACH };
			template<typename T>
			Basic::Ptr<T> Attach_Get_Detach_Component(MODE mode, Basic::Ptr<T> component = nullptr);

			std::string name;
			std::set<Basic::Ptr<Component>> components;
		};

#include <CppUtil/Engine/SObj.inl>
	}
}
#endif
