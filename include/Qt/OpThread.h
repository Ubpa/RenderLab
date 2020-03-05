#pragma once

#include <Basic/HeapObj.h>

#include <qthread.h>

namespace CppUtil {
	namespace Basic {
		class Op;
	}

	namespace QT {
		class OpThread : public QThread, public Basic::HeapObj {
			Q_OBJECT
		public:
			OpThread(Basic::Ptr<Basic::Op> op = nullptr);
			
		public:
			static const Basic::Ptr<OpThread> New(Basic::Ptr<Basic::Op> op = nullptr) {
				return Basic::New<OpThread>(op);
			}

		protected:
			virtual ~OpThread() = default;

		public:
			void SetOp(Basic::Ptr<Basic::Op> op) { this->op = op; }
			void Stop() { isStop = true; }
			bool IsStop() const { return isStop; }

		signals:
			void UI_Op(Basic::Ptr<Basic::Op> op);

		public:
			template<typename T>
			void UIConnect(T * obj, void (T::* f)(Basic::Ptr<Basic::Op> op)) {
				connect(this, &OpThread::UI_Op, obj, f);
			}
			void UI_Op_Run(Basic::Ptr<Basic::Op> op);

		protected:
			virtual void run();

		private:
			volatile bool isStop;
			Basic::Ptr<Basic::Op> op;
		};
	}
}
