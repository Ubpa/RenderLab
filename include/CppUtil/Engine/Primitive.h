#ifndef _ENGINE_PRIMITIVE_PRIMITIVE_H_
#define _ENGINE_PRIMITIVE_PRIMITIVE_H_

#include <CppUtil/Engine/Shape.h>

namespace CppUtil {
	namespace Engine {
		class Primitive : public Shape {
		protected:
			Primitive() = default;
			virtual ~Primitive() = default;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PRIMITIVE_H_