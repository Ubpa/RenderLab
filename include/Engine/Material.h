#ifndef _ENGINE_MATERIAL_MATERIAL_H_
#define _ENGINE_MATERIAL_MATERIAL_H_

#include <Basic/Element.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>
#include <UGM/point.h>
#include <UGM/rgb.h>
#include <Basic/Math.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {

		class Material : public Basic::Element {
		protected:
			Material() = default;
			virtual ~Material() = default;
		};
	}
}

#endif//!_ENGINE_MATERIAL_MATERIAL_H_
