#ifndef _CPPUTIL_ENGINE_MATERIAL_GOOCH_H_
#define _CPPUTIL_ENGINE_MATERIAL_GOOCH_H_

#include <Engine/Material.h>

#include <Basic/Image.h>

namespace CppUtil {
	namespace Engine {
		class Gooch : public Material {
		public:
			Gooch(const RGBf & colorFactor = RGBf(1.0f)) :colorFactor(colorFactor) {}
			virtual ~Gooch() = default;

		public:
			static Basic::Ptr<Gooch> New(const RGBf & colorFactor = RGBf(1.0f)) {
				return Basic::New<Gooch>(colorFactor);
			}

		public:
			RGBf colorFactor;
			Basic::Ptr<Basic::Image> colorTexture;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MATERIAL_GOOCH_H_
