#ifndef _OP_THREAD_H_
#define _OP_THREAD_H_

#include <CppUtil/Basic/HeapObj.h>

#include <qthread.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}

	namespace Qt {
		class OpThread : public QThread, public Basic::HeapObj {
			HEAP_OBJ_SETUP(OpThread)

			public:
				OpThread(Basic::Ptr<Basic::Operation> op = nullptr);
				void SetOp(Basic::Ptr<Basic::Operation> op) { this->op = op; }
				void Stop() { isStop = true; }
				bool IsStop() const { return isStop; }

			protected:
				virtual void run();

			private:
				volatile bool isStop;
				Basic::Ptr<Basic::Operation> op;
		};
	}
}

#endif // !_OP_THREAD_H_
