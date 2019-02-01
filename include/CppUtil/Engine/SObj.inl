
template<typename T>
void SObj::AttachComponent(Basic::Ptr<T> component) {
	Attach_Get_Detach_Component<T>(MODE::ATTACH, component);
}

template<typename T>
CppUtil::Basic::Ptr<T> SObj::GetComponent(T *) {
	return Attach_Get_Detach_Component<T>(MODE::GET);
}

template<typename T>
void SObj::DetachComponent(T * useless_parameter) {
	Attach_Get_Detach_Component<T>(MODE::DETACH);
}

template<typename T>
CppUtil::Basic::Ptr<T> SObj::Attach_Get_Detach_Component(MODE mode, Basic::Ptr<T> component) {
	// 这里使用 weak_ptr，不含有所有权
	static Basic::LStorage<Basic::WPtr<SObj>, Basic::WPtr<T>> storage;
	Basic::WPtr<T> tmpComponent;
	switch (mode)
	{
	case ATTACH: // Attach
		if (component != nullptr) {
			storage.GetV(This(), tmpComponent);
			if (!tmpComponent.expired())
				tmpComponent.lock()->Detach();

			components.insert(component);
			storage.Reg(This(), component);
		}
		return component;
	case GET: // Get
		if (!storage.GetV(This(), tmpComponent))
			return nullptr;
		else
			return tmpComponent.lock();
	case DETACH: // Detach
		if (storage.GetV(This(), tmpComponent)) {
			Basic::Ptr<T> conponent = tmpComponent.lock();
			components.erase(conponent);
			storage.UnReg(This());
		}

		return nullptr;
	default:
		return nullptr;
	}
}