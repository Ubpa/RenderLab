#ifndef _OP_THREAD_H_
#define _OP_THREAD_H_

#include <CppUtil/Basic/HeapObj.h>

#include <qthread.h>

#include <functional>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}

	namespace Qt {
		class OpThread : public QThread, public Basic::HeapObj {
			Q_OBJECT

			HEAP_OBJ_SETUP(OpThread)

			public:
				OpThread(Basic::Ptr<Basic::Operation> op = nullptr);
				OpThread(std::function<void()> op);
				void SetOp(Basic::Ptr<Basic::Operation> op) { this->op = op; }
				void SetOp(std::function<void()> op);
				void Stop() { isStop = true; }
				bool IsStop() const { return isStop; }

			signals:
				void UI_Op(Basic::Ptr<Basic::Operation> op);

			public:
				template<typename T>
				void UIConnect(T * obj, void (T::* f)(Basic::Ptr<Basic::Operation> op)) {
					connect(this, &OpThread::UI_Op, obj, f);
				}
				void UI_Op_Run(Basic::Ptr<Basic::Operation> op);
				void UI_Op_Run(std::function<void()> op);

			protected:
				virtual void run();

			private:
				volatile bool isStop;
				Basic::Ptr<Basic::Operation> op;
		};
	}
}

#endif // !_OP_THREAD_H_
