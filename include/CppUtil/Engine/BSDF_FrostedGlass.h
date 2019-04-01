#ifndef _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
#define _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class BSDF_FrostedGlass : public Engine::BSDF {
			ELE_SETUP(BSDF_FrostedGlass)
		public:
			BSDF_FrostedGlass(const RGBf & colorFactor = RGBf(1.f), float roughnessFactor = 1.f, float ior = 1.5f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), ior(ior),
				colorTexture(nullptr),
				roughnessTexture(nullptr),
				aoTexture(nullptr),
				normalTexture(nullptr) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD);

			virtual void ChangeNormal(const Point2f & texcoord, const Normalf & tangent, Normalf & normal) const;

		private:
			static float GGX_D(const Normalf & h, float alpha);
			static float GGX_G1(const Normalf & v, const Normalf & h, float alpha);
			static float GGX_G(const Normalf & wo, const Normalf & wi, const Normalf & h, float alpha);
			static float Fr(const Normalf & v, const Normalf & h, float ior);

		private:
			const RGBf GetColor(const Point2f & texcoord) const;
			float GetRoughness(const Point2f & texcoord) const;
			float GetAO(const Point2f & texcoord) const;

		public:
			RGBf colorFactor;
			Basic::Ptr<Basic::Image> colorTexture;

			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture;
			Basic::Ptr<Basic::Image> normalTexture;

			float ior;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
