#pragma once

#include <Basic/HeapObj.h>

#include <qthread.h>

namespace Ubpa {
	class Op;

	class OpThread : public QThread, public HeapObj {
		Q_OBJECT
	public:
		OpThread(Ptr<Op> op = nullptr);

	public:
		static const Ptr<OpThread> New(Ptr<Op> op = nullptr) {
			return Ubpa::New<OpThread>(op);
		}

	protected:
		virtual ~OpThread() = default;

	public:
		void SetOp(Ptr<Op> op) { this->op = op; }
		void Stop() { isStop = true; }
		bool IsStop() const { return isStop; }

	signals:
		void UI_Op(Ptr<Op> op);

	public:
		template<typename T>
		void UIConnect(T* obj, void (T::* f)(Ptr<Op> op)) {
			connect(this, &OpThread::UI_Op, obj, f);
		}
		void UI_Op_Run(Ptr<Op> op);

	protected:
		virtual void run();

	private:
		volatile bool isStop;
		Ptr<Op> op;
	};
}
