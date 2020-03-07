#pragma once

#include <Basic/LStorage.h>
#include <Basic/Node.h>
#include <Basic/TypeMap.h>
#include <UGM/transform.h>

#include <string>
#include <vector>
#include <functional>

namespace Ubpa {
	class Component;

	// 组件式编程，不要再派生子类了，用 component 来表达语义
	class SObj final : public Node<SObj> {
	private:
		template<typename T>
		using enable_if_is_component_t = std::enable_if_t<std::is_base_of_v<Component, T>>;

	public:
		SObj(Ptr<SObj> parent = nullptr, const std::string& name = "SObj")
			: Node(parent), name(name) { }

		bool Save(const std::string& path);

	public:
		static const Ptr<SObj> New(Ptr<SObj> parent = nullptr, const std::string& name = "SObj") {
			return Ubpa::New<SObj>(parent, name);
		}

		static const Ptr<SObj> Load(const std::string& path);

	protected:
		virtual ~SObj() = default;

	public:
		template<typename T, typename = enable_if_is_component_t<T>>
		const Ptr<T> GetComponent() const;

		template<typename T, typename = enable_if_is_component_t<T>>
		bool HaveComponent() const { return components.find(typeid(T)) != components.cend(); }
		bool HaveComponentSameTypeWith(Ptr<Component> ptr) const;

		const std::vector<Ptr<Component>> GetAllComponents() const;

		template<typename T, typename ...Args, typename = enable_if_is_component_t<T>>
		const Ptr<T> AddComponent(Args&& ... args);

		void AttachComponent(Ptr<Component> component);

		template<typename T, typename = enable_if_is_component_t<T>>
		bool DetachComponent();

		bool DetachComponent(Ptr<Component> component);

		template<typename T, typename = enable_if_is_component_t<T>>
		const Ptr<T> GetComponentInChildren();

		template<typename T, typename = enable_if_is_component_t<T>>
		const std::vector<Ptr<T>> GetComponentsInChildren();

	public:
		// 即时计算
		const transformf GetLocalToWorldMatrix();
		const pointf3 GetWorldPos() { return GetLocalToWorldMatrix() * pointf3(0.f, 0.f, 0.f); }

	public:
		std::string name;

	private:
		TypeMap<Ptr<Component>> components;
	};

#include <Engine/Scene/SObj.inl>
}
