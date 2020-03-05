#pragma once

#include <Basic/Element.h>

namespace Ubpa {
	class SObj;

	class Component : public Element {
	protected:
		Component(Ptr<SObj> sobj) : wSObj(sobj) { }
		virtual ~Component() = default;

	protected:
		virtual void Init_AfterGenPtr() override;

	public:
		const Ptr<SObj> GetSObj() const { return wSObj.lock(); }

	private:
		friend SObj;
		WPtr<SObj> wSObj;
	};
}
