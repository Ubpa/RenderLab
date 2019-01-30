#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class Primitive : public Basic::Element {
			ELE_SETUP(Primitive)
		protected:
			Primitive() = default;
		};
	}
}

#endif//!_PRIMITIVE_H_