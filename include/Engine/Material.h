#ifndef _ENGINE_MATERIAL_MATERIAL_H_
#define _ENGINE_MATERIAL_MATERIAL_H_

#include <Basic/Element.h>
#include <Basic/UGM/Point2.h>
#include <Basic/UGM/Vector.h>
#include <Basic/UGM/Normal.h>
#include <Basic/UGM/Point.h>
#include <Basic/UGM/RGB.h>
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
