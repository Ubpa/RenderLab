#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class Op : public HeapObj {
	protected:
		Op(bool isHold = true) :isHold(isHold) { }
		virtual ~Op() = default;

	public:
		void operator()() { Run(); }
		virtual void Run() = 0;

	public:
		bool isHold;
	};
}
