template<typename T>
struct SObj::Wrapper<T, true> {
	static void Detach(Basic::TypeMap<Basic::Ptr<ComponentBase>> & components, Basic::Ptr<T> component) {
		components.erase(typeid(*component));
	}

	static void Detach(Basic::TypeMap<Basic::Ptr<ComponentBase>> & components) {
		components.erase(typeid(T));
	}

	static Basic::Ptr<T> GetComponent(const Basic::TypeMap<Basic::Ptr<ComponentBase>> & components) {
		auto target = components.find(typeid(T));
		if (target == components.end())
			return nullptr;

		return T::PtrCast(target->second);
	}
};

template<typename T>
const CppUtil::Basic::Ptr<T> SObj::GetComponent(T *) const {
	return Wrapper<T, std::is_base_of<ComponentBase, T>::value>::GetComponent(components);
}

template<typename T>
void SObj::DetachComponent(T *) {
	Wrapper<T, std::is_base_of<ComponentBase, T>::value>::Detach(components);
}

template<typename T>
void SObj::DetachComponent(Basic::Ptr<T> component) {
	Wrapper<T, std::is_base_of<ComponentBase, T>::value>::Detach(components, component);
}

template<typename T>
const CppUtil::Basic::Ptr<T> SObj::GetComponentInChildren() {
	auto visitor = Basic::VisitorDynamic::New();

	Basic::Ptr<T> componentOfT = nullptr;
	visitor->Reg([&componentOfT, visitor](Basic::Ptr<SObj> sobj) {
		// 因为 GetComponent<T>() 会检测 T 是否以 Component 为基类，所以不需要用 std::is_base_of 来判断
		auto tmpComponent = sobj->GetComponent<T>();
		if (tmpComponent) {
			componentOfT = tmpComponent;
			return;
		}
		
		for (auto child : sobj->GetChildren()) {
			child->Accept(visitor);
			if (componentOfT != nullptr)
				return;
		}
	});
	Accept(visitor);
	return componentOfT;
}

template<typename T>
const std::vector<CppUtil::Basic::Ptr<T> > SObj::GetComponentsInChildren() {
	auto visitor = Basic::VisitorDynamic::New();

	std::vector<Basic::Ptr<T>> componentsOfT;

	visitor->Reg([&componentsOfT](Basic::Ptr<SObj> sobj) {
		// 因为 GetComponent<T>() 会检测 T 是否以 Component 为基类，所以不需要用 std::is_base_of 来判断
		auto componentOfT = sobj->GetComponent<T>();
		if (componentOfT)
			componentsOfT.push_back(componentOfT);
	});
	TraverseAccept(visitor);
	return componentsOfT;
}