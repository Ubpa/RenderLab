#ifndef _ENGINE_MATERIAL_MATERIAL_H_
#define _ENGINE_MATERIAL_MATERIAL_H_

#include <CppUtil/Basic/Element.h>
#include <CppUtil/Basic/UGM/Point2.h>
#include <CppUtil/Basic/UGM/Vector.h>
#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/Point.h>
#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/Math.h>

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
