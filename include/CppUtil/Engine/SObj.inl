template<typename T>
struct SObj::Wrapper<T, true> {
	static void Detach(Basic::TypeMap<Basic::Ptr<Component>> & components, Basic::Ptr<T> component) {
		components.erase(typeid(*component));
	}

	static void Detach(Basic::TypeMap<Basic::Ptr<Component>> & components) {
		components.erase(typeid(T));
	}

	static Basic::Ptr<T> GetComponent(const Basic::TypeMap<Basic::Ptr<Component>> & components) {
		auto target = components.find(typeid(T));
		return target == components.end() ? nullptr : target->second;
	}
};

template<typename T>
CppUtil::Basic::Ptr<T> SObj::GetComponent(T *) const {
	return Wrapper<T, std::is_base_of<Component, T>::value>::GetComponent(components);
}

template<typename T>
void SObj::DetachComponent(T *) {
	Wrapper<T, std::is_base_of<Component, T>::value>::Detach(components);
}

template<typename T>
void SObj::DetachComponent(Basic::Ptr<T> component) {
	Wrapper<T, std::is_base_of<Component, T>::value>::Detach(components, component);
}

template<typename T>
CppUtil::Basic::Ptr<T> SObj::GetComponentInChildren() {
	Basic::Ptr<T> componentT = nullptr;
	auto visitor = ToPtr(new Basic::EleVisitor);
	visitor->Reg<SObj>([&componentT](SObj::Ptr sobj)->bool {
		// 因为 GetComponent<T>() 会检测 T 是否以 Component 为基类，所以不需要用 std::is_base_of 来判断
		auto tmpComponent = sobj->GetComponent<T>();
		if (tmpComponent) {
			componentT = tmpComponent;
			return false;
		}
		return true;
	});
	TraverseAccept(visitor);
	return componentT;
}

template<typename T>
std::vector<CppUtil::Basic::Ptr<T> > SObj::GetComponentsInChildren() {
	std::vector<Basic::Ptr<T>> componentsT;
	auto visitor = ToPtr(new Basic::EleVisitor);

	visitor->Reg<SObj>([&componentsT](SObj::Ptr sobj)->bool {
		// 因为 GetComponent<T>() 会检测 T 是否以 Component 为基类，所以不需要用 std::is_base_of 来判断
		auto tmpComponent = sobj->GetComponent<T>();
		if (tmpComponent)
			componentsT.push_back(tmpComponent);

		return true;
	});
	TraverseAccept(visitor);
	return componentsT;
}