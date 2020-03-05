#pragma once

#include <Engine/Shape.h>

namespace CppUtil {
	namespace Engine {
		// renderable
		class Primitive : public Shape {
		protected:
			Primitive() = default;
			virtual ~Primitive() = default;

		public:
			virtual const Basic::Ptr<Primitive> GetPrimitive() override {
				return This<Primitive>();
			}
		};
	}
}
