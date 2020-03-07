#pragma once

#include "Op.h"

namespace Ubpa {
	// 要求无参数
	template<typename LambdaExpr>
	class LambdaOp : public Op {
	public:
		LambdaOp(const LambdaExpr& op, bool isHold) : op(op), Op(isHold) { }

	protected:
		virtual ~LambdaOp() = default;

	public:
		virtual void Run() override { op(); }

	private:
		LambdaExpr op;
	};

	template<typename LambdaExpr>
	const Ptr<LambdaOp<LambdaExpr>> LambdaOp_New(const LambdaExpr& op, bool isHold = false) {
		return Ubpa::New<LambdaOp<LambdaExpr>>(op, isHold);
	}
}
