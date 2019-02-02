#ifndef _SOBJ_H_
#define _SOBJ_H_ 

#include <CppUtil/Basic/LStorage.h>
#include <CppUtil/Basic/Node.h>
#include <CppUtil/Basic/TypeMap.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class Component;

		class SObj : public CppUtil::Basic::Node {
			ELE_SETUP_SELF_DEL(SObj)
		public:
			SObj(const std::string & name = "")
				: name(name) { }

		protected:
			~SObj();

		public:
			void AttachComponent(Basic::Ptr<Component> component);

			template<typename T>
			Basic::Ptr<T> GetComponent(T * useless_parameter = nullptr) {
				auto target = components.find(typeid(T));
				return target == components.end() ? nullptr : target->second;
			}

			std::vector<Basic::Ptr<Component>> GetAllComponents() const;

			template<typename T>
			void DetachComponent(T * useless_parameter = nullptr) {
				components.erase(typeid(T));
			}

			void DetachComponent(Basic::Ptr<Component> component) {
				components.erase(typeid(component));
			}

		private:
			Basic::TypeMap<Basic::Ptr<Component>> components;

		public:
			std::string name;
		};
	}
}
#endif
