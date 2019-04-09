#ifndef _ENGINE_MATERIAL_MATERIAL_H_
#define _ENGINE_MATERIAL_MATERIAL_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class Material : public Basic::Element {
		protected:
			Material() = default;
			virtual ~Material() = default;
		};
	}
}

#endif//!_ENGINE_MATERIAL_MATERIAL_H_
