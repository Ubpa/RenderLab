#pragma once

#include <Basic/Op.h>

namespace CppUtil {
	namespace Basic {
		// 要求无参数
		template<typename LambdaExpr>
		class LambdaOp : public Op {
		public:
			LambdaOp(const LambdaExpr & op, bool isHold) : op(op), Op(isHold) { }

		protected:
			virtual ~LambdaOp() = default;

		public:
			virtual void Run() override { op(); }

		private:
			LambdaExpr op;
		};

		template<typename LambdaExpr>
		const Ptr<LambdaOp<LambdaExpr>> LambdaOp_New(const LambdaExpr & op, bool isHold = false) {
			return CppUtil::Basic::New<LambdaOp<LambdaExpr>>(op, isHold);
		}
	}
}
