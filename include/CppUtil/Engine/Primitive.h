#ifndef _ENGINE_PRIMITIVE_PRIMITIVE_H_
#define _ENGINE_PRIMITIVE_PRIMITIVE_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class PrimitiveBase : public Basic::ElementBase {
		protected:
			PrimitiveBase() = default;
			virtual ~PrimitiveBase() = default;
		};

		template<typename ImplT>
		class Primitive : public Basic::Element<ImplT, PrimitiveBase> {
		protected:
			using Element<ImplT, PrimitiveBase>::Element;
			virtual ~Primitive() = default;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_PRIMITIVE_H_