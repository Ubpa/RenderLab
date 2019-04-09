#ifndef _ENGINE_PRIMITIVE_PRIMITIVE_H_
#define _ENGINE_PRIMITIVE_PRIMITIVE_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class Primitive : public Basic::Element {
		protected:
			Primitive() = default;
			virtual ~Primitive() = default;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PRIMITIVE_H_