#ifndef _ENGINE_SCENE_SOBJ_H_
#define _ENGINE_SCENE_SOBJ_H_ 

#include <CppUtil/Basic/LStorage.h>
#include <CppUtil/Basic/Node.h>
#include <CppUtil/Basic/TypeMap.h>
#include <CppUtil/Basic/EleVisitor.h>

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class Component;

		// 组件式编程，不要再派生子类了，就是用 component 来表达语义
		class SObj final : public Basic::Node {
			NODE_SETUP(SObj)
		public:
			SObj(Node::Ptr parent = nullptr, const std::string & name = "")
				: Node(parent), name(name) { }

		public:
			void AttachComponent(Basic::Ptr<Component> component);

			template<typename T>
			Basic::Ptr<T> GetComponent(T * useless_parameter = nullptr) const;

			std::vector<Basic::Ptr<Component>> GetAllComponents() const;

			template<typename T>
			void DetachComponent(T * useless_parameter = nullptr);

			template<typename T>
			void DetachComponent(Basic::Ptr<T> component);

			template<typename T>
			Basic::Ptr<T> GetComponentInChildren();

			template<typename T>
			std::vector<Basic::Ptr<T>> GetComponentsInChildren();


		public:
			// 即时计算
			glm::mat4 GetLocalToWorldMatrix();
			glm::mat4 GetWorldToLocalMatrix() {
				return glm::inverse(GetLocalToWorldMatrix());
			}

		public:
			std::string name;

		private:
			template<typename T, bool T_is_base_of_Component>
			struct Wrapper;

		private:
			Basic::TypeMap<Basic::Ptr<Component>> components;
		};

#include <CppUtil/Engine/SObj.inl>
	}
}
#endif//!_ENGINE_SCENE_SOBJ_H_
