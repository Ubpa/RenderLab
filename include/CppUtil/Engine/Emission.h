#ifndef _CPPUTIL_ENGINE_MATERIAL_EMISSION_H_
#define _CPPUTIL_ENGINE_MATERIAL_EMISSION_H_

#include <CppUtil/Engine/Material.h>

namespace CppUtil {
	namespace Engine {
		class Emission : public Material {
		public:
			Emission(const RGBf & color = 1, float intensity = 1.f)
				: color(color), intensity(intensity) { }

		public:
			static Basic::Ptr<Emission> New(const RGBf & color = 1, float intensity = 1.f) {
				return Basic::New<Emission>(color, intensity);
			}

		public:
			virtual const RGBf GetEmission() const override { return intensity * color; }

		public:
			RGBf color;
			float intensity;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_MATERIAL_EMISSION_H_
