#ifndef _OPNODE_H_
#define _OPNODE_H_

#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/LambdaOp.h>

namespace CppUtil {
	namespace Basic {
		class OpNode : public OpQueue {
			HEAP_OBJ_SETUP(OpNode)
		public:
			OpNode(Operation::Ptr preOp = nullptr, Operation::Ptr postOp = nullptr, bool isHold = true);
			OpNode(const std::function<void()> & preOp = nullptr, const std::function<void()> & postOp = nullptr, bool isHold = true);
			//------------
			void SetPreOp(Operation::Ptr preOp);
			void SetPostOp(Operation::Ptr postOp);
			size_t ChildNum() const;
			virtual void Run();
		private:
			using OpQueue::Size;
			OpNode(const OpNode &) = delete;
			OpNode& operator=(const OpNode &) = delete;

			Operation::Ptr preOp;
			Operation::Ptr postOp;
		};
	}
}

#endif // !_OPNODE_H_
