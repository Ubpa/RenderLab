#ifndef _CPPUTIL_BASIC_OPERATION_OP_QUEUE_H_
#define _CPPUTIL_BASIC_OPERATION_OP_QUEUE_H_

#include <CppUtil/Basic/Op.h>
#include <CppUtil/Basic/HeapObj.h>
#include <list>

namespace CppUtil {
	namespace Basic {
		class OpQueue final : public Op, public HeapObj<OpQueue> {
		public:
			OpQueue(bool isHold = true) : Op(isHold) { }
			
		public:
			static const Ptr<OpQueue> New(bool isHold = true) { return CppUtil::Basic::New<OpQueue>(isHold); }

		public:
			void Push(Basic::Ptr<Op> op);
			OpQueue & operator<<(Basic::Ptr<Op> op) {
				Push(op);
				return *this;
			}
			
			virtual void Run() override;

		private:
			virtual ~OpQueue() = default;

		private:
			std::list<Basic::Ptr<Op>> opList;
		};
	}
}

#endif // !_CPPUTIL_BASIC_OPERATION_OP_QUEUE_H_
