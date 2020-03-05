#pragma once

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
