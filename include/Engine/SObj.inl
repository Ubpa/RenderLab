template<typename T, typename>
const Ptr<T> SObj::GetComponent() const {
	auto target = components.find(typeid(T));
	if (target == components.end())
		return nullptr;

	return CastTo<T>(target->second);
}

template<typename T, typename>
bool SObj::DetachComponent() {
	auto target = components.find(typeid(T));
	if (target == components.end())
		return false;

	target->second->wSObj.reset();
	return true;
}

template<typename T, typename>
const Ptr<T> SObj::GetComponentInChildren() {
	auto visitor = Visitor::New();

	Ptr<T> componentOfT = nullptr;
	visitor->Reg([&componentOfT, visitor](Ptr<SObj> sobj) {
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

template<typename T, typename>
const std::vector<Ptr<T> > SObj::GetComponentsInChildren() {
	auto visitor = Visitor::New();

	std::vector<Ptr<T>> componentsOfT;

	visitor->Reg([&componentsOfT](Ptr<SObj> sobj) {
		auto componentOfT = sobj->GetComponent<T>();
		if (componentOfT)
			componentsOfT.push_back(componentOfT);
	});
	TraverseAccept(visitor);
	return componentsOfT;
}

template<typename T, typename ...Args, typename>
const Ptr<T> SObj::AddComponent(Args && ... args) {
	if (HaveComponent<T>())
		return nullptr;

	auto cmpt = T::New(This<SObj>(), std::forward<Args>(args)...);
	return cmpt;
}
