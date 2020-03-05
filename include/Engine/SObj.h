#ifndef _ENGINE_SCENE_SOBJ_H_
#define _ENGINE_SCENE_SOBJ_H_ 

#include <Basic/LStorage.h>
#include <Basic/Node.h>
#include <Basic/TypeMap.h>
#include <Basic/Visitor.h>
#include <UGM/transform.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class Component;

		// 组件式编程，不要再派生子类了，用 component 来表达语义
		class SObj final : public Basic::Node<SObj> {
		private:
			template<typename T>
			using enable_if_is_component_t = std::enable_if_t<std::is_base_of_v<Component, T>>;

		public:
			SObj(Basic::Ptr<SObj> parent = nullptr, const std::string & name = "SObj")
				: Node(parent), name(name) { }

			bool Save(const std::string & path);

		public:
			static const Basic::Ptr<SObj> New(Basic::Ptr<SObj> parent = nullptr, const std::string & name = "SObj") {
				return Basic::New<SObj>(parent, name);
			}

			static const Basic::Ptr<SObj> Load(const std::string & path);

		protected:
			virtual ~SObj() = default;

		public:
			template<typename T, typename = enable_if_is_component_t<T>>
			const Basic::Ptr<T> GetComponent() const;

			template<typename T, typename = enable_if_is_component_t<T>>
			bool HaveComponent() const { return components.find(typeid(T)) != components.cend(); }
			bool HaveComponentSameTypeWith(Basic::Ptr<Component> ptr) const;

			const std::vector<Basic::Ptr<Component>> GetAllComponents() const;

			template<typename T, typename ...Args, typename = enable_if_is_component_t<T>>
			const Basic::Ptr<T> AddComponent(Args && ... args);

			void AttachComponent(Basic::Ptr<Component> component);

			template<typename T, typename = enable_if_is_component_t<T>>
			bool DetachComponent();

			bool DetachComponent(Basic::Ptr<Component> component);

			template<typename T, typename = enable_if_is_component_t<T>>
			const Basic::Ptr<T> GetComponentInChildren();

			template<typename T, typename = enable_if_is_component_t<T>>
			const std::vector<Basic::Ptr<T>> GetComponentsInChildren();

		public:
			// 即时计算
			const Ubpa::transformf GetLocalToWorldMatrix();
			const Ubpa::pointf3 GetWorldPos() { return GetLocalToWorldMatrix() * Ubpa::pointf3(0.f,0.f,0.f); }

		public:
			std::string name;

		private:
			Basic::TypeMap<Basic::Ptr<Component>> components;
		};

#include <Engine/SObj.inl>
	}
}
#endif//!_ENGINE_SCENE_SOBJ_H_
