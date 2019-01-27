#ifndef _OP_QUEUE_H_
#define _OP_QUEUE_H_

#include <CppUtil/Basic/Operation.h>
#include <list>

namespace CppUtil {
	namespace Basic {
		class OpQueue : public Operation {
			HEAP_OBJ_SETUP(OpQueue)
		public:
			OpQueue(bool isHold = true);
			//------------
			OpQueue & operator<<(Operation::Ptr operation);
			void Push(Operation::Ptr op);
			size_t Size() const;
			//------------
			virtual void Run();
		private:
			OpQueue(const OpQueue &) = delete;
			OpQueue& operator=(const OpQueue &) = delete;

			std::list< Operation::Ptr > opList;
		};
	}
}

#endif // !_OP_QUEUE_H_
