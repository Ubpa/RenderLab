#ifndef _CPPUTIL_BASIC_OPERATION_LAMBDA_OP_H_
#define _CPPUTIL_BASIC_OPERATION_LAMBDA_OP_H_

#include <CppUtil/Basic/Op.h>

namespace CppUtil {
	namespace Basic {
		// 要求无参数
		template<typename LambdaExpr>
		class LambdaOp : public Op {
		public:
			LambdaOp(const LambdaExpr & op, bool isHold = true) : op(op), Op(isHold) { }

		protected:
			virtual ~LambdaOp() = default;

		public:
			virtual void Run() override { op(); }

		private:
			LambdaExpr op;
		};

		template<typename LambdaExpr>
		const Ptr<LambdaOp<LambdaExpr>> LambdaOp_New(const LambdaExpr & op, bool isHold = true) {
			return CppUtil::Basic::New<LambdaOp<LambdaExpr>>(op, isHold);
		}
	}
}

#endif // !_CPPUTIL_BASIC_OPERATION_LAMBDA_OP_H_
