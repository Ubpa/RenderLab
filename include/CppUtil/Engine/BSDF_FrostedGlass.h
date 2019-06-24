#ifndef _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_
#define _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTED_GLASS_H_

#include <CppUtil/Engine/BSDF.h>
#include <CppUtil/Engine/GGX.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_FrostedGlass : public BSDF {
		public:
			BSDF_FrostedGlass(const RGBf & colorFactor = RGBf(1.f), float roughnessFactor = 1.f, float ior = 1.5f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), ior(ior),
				colorTexture(nullptr),
				roughnessTexture(nullptr),
				aoTexture(nullptr),
				normalTexture(nullptr) { }

		public:
			static const Basic::Ptr<BSDF_FrostedGlass> New(const RGBf & colorFactor = RGBf(1.f), float roughnessFactor = 1.f, float ior = 1.5f) {
				return Basic::New<BSDF_FrostedGlass>(colorFactor, roughnessFactor, ior);
			}

		protected:
			virtual ~BSDF_FrostedGlass() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override;

			virtual void ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const override;

		private:
			static float Fr(const Normalf & v, const Normalf & h, float ior);

		private:
			const RGBf GetColor(const Point2 & texcoord) const;
			float GetRoughness(const Point2 & texcoord) const;
			float GetAO(const Point2 & texcoord) const;

		public:
			GGX ggx;

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
