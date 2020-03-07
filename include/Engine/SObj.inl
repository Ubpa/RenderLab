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
	std::function<Ptr<T>(Ptr<SObj>)> getComponent;
	getComponent = [&getComponent](Ptr<SObj> sobj)->Ptr<T> {
		auto cmpt = sobj->GetComponent<T>();
		if (cmpt)
			return cmpt;

		for (auto child : sobj->GetChildren()) {
			auto cmpt = getComponent(child);
			if (cmpt)
				return cmpt;
		}

		return nullptr;
	};
	return getComponent(This<SObj>());
}

template<typename T, typename>
const std::vector<Ptr<T>> SObj::GetComponentsInChildren() {
	std::vector<Ptr<T>> rst;
	std::function<void(Ptr<SObj>)> collectCmpt;
	collectCmpt = [&rst, &collectCmpt](Ptr<SObj> sobj) {
		auto cmpt = sobj->GetComponent<T>();
		if (cmpt)
			rst.push_back(cmpt);

		for (auto child : sobj->GetChildren())
			collectCmpt(child);
	};

	collectCmpt(This<SObj>());
	return rst;
}

template<typename T, typename ...Args, typename>
const Ptr<T> SObj::AddComponent(Args && ... args) {
	if (HaveComponent<T>())
		return nullptr;

	auto cmpt = T::New(This<SObj>(), std::forward<Args>(args)...);
	return cmpt;
}
