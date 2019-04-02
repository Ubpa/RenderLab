#ifndef _ENGINE_SCENE_SOBJ_H_
#define _ENGINE_SCENE_SOBJ_H_ 

#include <CppUtil/Basic/LStorage.h>
#include <CppUtil/Basic/Node.h>
#include <CppUtil/Basic/TypeMap.h>
#include <CppUtil/Basic/EleVisitor.h>
#include <CppUtil/Basic/Transform.h>

#include <string>
#include <vector>

namespace CppUtil {
	namespace Engine {
		class Component;

		// 组件式编程，不要再派生子类了，就是用 component 来表达语义
		class SObj final : public Basic::Node<SObj> {
			ELE_SETUP(SObj)
		public:
			SObj(SObj::Ptr parent = nullptr, const std::string & name = "")
				: Node(parent), name(name) { }

			bool Save(const std::string & path);
			static const SObj::Ptr Load(const std::string & path);

		public:
			template<typename T>
			const Basic::Ptr<T> GetComponent(T * useless_parameter = nullptr) const;

			bool HaveComponentSameTypeWith(Basic::Ptr<Component> ptr) const;

			const std::vector<Basic::Ptr<Component>> GetAllComponents() const;

			void AttachComponent(Basic::Ptr<Component> component);

			template<typename T>
			void DetachComponent(T * useless_parameter = nullptr);

			template<typename T>
			void DetachComponent(Basic::Ptr<T> component);

			template<typename T>
			const Basic::Ptr<T> GetComponentInChildren();

			template<typename T>
			const std::vector<Basic::Ptr<T>> GetComponentsInChildren();

		public:
			// 即时计算
			const Basic::Transform GetLocalToWorldMatrix();
			const Basic::Transform GetWorldToLocalMatrix() {
				return GetLocalToWorldMatrix().Inverse();
			}
			const Point3 GetWorldPos() {
				return GetLocalToWorldMatrix()(Point3(0));
			}

		public:
			virtual void InitAfterGenSharePtr() {
				Basic::Node<SObj>::InitAfterGenSharePtr();
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
