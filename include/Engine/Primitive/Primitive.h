#pragma once

#include "Shape.h"

namespace Ubpa {
	// renderable
	class Primitive : public Shape {
	protected:
		Primitive() = default;
		virtual ~Primitive() = default;

	public:
		virtual const Ptr<Primitive> GetPrimitive() override {
			return This<Primitive>();
		}
	};
}
