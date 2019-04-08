#ifndef _ENGINE_MATERIAL_MATERIAL_H_
#define _ENGINE_MATERIAL_MATERIAL_H_

#include <CppUtil/Basic/Element.h>

namespace CppUtil {
	namespace Engine {
		class MaterialBase : public Basic::ElementBase {
		protected:
			MaterialBase() = default;
			virtual ~MaterialBase() = default;
		};

		template<typename ImplT, typename BaseT = MaterialBase>
		class Material : public Basic::Element<ImplT, BaseT> {
		protected:
			Material() = default;
			virtual ~Material() = default;
		};
	}
}

#endif//!_ENGINE_MATERIAL_MATERIAL_H_