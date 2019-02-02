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

			const std::string & GetName() const { return name; }
			void SetName(const std::string & name) { this->name = name; }

		protected:
			~SObj();

		private:
			using TypeInfoRef = std::reference_wrapper<const std::type_info>;

			struct Hasher {
				std::size_t operator()(TypeInfoRef code) const
				{
					return code.get().hash_code();
				}
			};

			struct EqualTo {
				bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const
				{
					return lhs.get() == rhs.get();
				}
			};

			Basic::TypeMap<Basic::Ptr<Component>> components;
			std::string name;
		};
	}
}
#endif
