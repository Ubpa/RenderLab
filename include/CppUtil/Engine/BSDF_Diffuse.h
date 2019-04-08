#ifndef _ENGINE_MATERIAL_BSDF_DIFFUSE_H_
#define _ENGINE_MATERIAL_BSDF_DIFFUSE_H_

#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class BSDF_Diffuse final : public BSDF<BSDF_Diffuse> {
		public:
			BSDF_Diffuse(const RGBf & colorFactor = RGBf(1))
				: colorFactor(colorFactor), albedoTexture(nullptr) { }

		public:
			static const Basic::Ptr<BSDF_Diffuse> New(const RGBf & colorFactor = RGBf(1)) {
				return Basic::New<BSDF_Diffuse>(colorFactor);
			}

		protected:
			virtual ~BSDF_Diffuse() = default;

		public:
			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) override;

		private:
			const RGBf GetAlbedo(const Point2 & texcoord) const;

		public:
			RGBf colorFactor;
			Basic::Ptr<Basic::Image> albedoTexture;

		private:
			Basic::CosineWeightedHemisphereSampler3D sampler;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_DIFFUSE_H_