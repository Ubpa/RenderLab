#pragma once

#include "Op.h"

#include <list>

namespace Ubpa {
	class OpQueue final : public Op {
	public:
		OpQueue(bool isHold = true) : Op(isHold) { }

	public:
		static const Ptr<OpQueue> New(bool isHold = true) { return Ubpa::New<OpQueue>(isHold); }

	public:
		void Push(Ptr<Op> op);
		OpQueue& operator<<(Ptr<Op> op) {
			Push(op);
			return *this;
		}

		virtual void Run() override;

		bool IsEmpty() const { return opList.empty(); }

	private:
		virtual ~OpQueue() = default;

	private:
		std::list<Ptr<Op>> opList;
	};
}
