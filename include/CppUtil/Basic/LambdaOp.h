#ifndef _LAMBDA_OP_H_
#define _LAMBDA_OP_H_

#include <CppUtil/Basic/Operation.h>
#include <functional>

namespace CppUtil {
	namespace Basic {
		class LambdaOp : public Operation {
			HEAP_OBJ_SETUP(LambdaOp)
		public:
			LambdaOp(const std::function<void()> & op = []() {}, bool isHold = true);
			void SetOp(const std::function<void()> & op);
			//------------
			virtual void Run();
		private:
			LambdaOp(const LambdaOp&) = delete;
			LambdaOp& operator=(const LambdaOp&) = delete;

			std::function<void()> op;
		};
	}
}

#endif // !_LAMBDA_OP_H_
