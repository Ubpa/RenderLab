#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class SObj;

	class Component : public HeapObj {
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
