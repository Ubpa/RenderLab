#ifndef _ENGINE_MATERIAL_BSDF_DIFFUSE_H_
#define _ENGINE_MATERIAL_BSDF_DIFFUSE_H_

#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class BSDF_Diffuse : public BSDF {
			ELE_SETUP(BSDF_Diffuse)
		public:
			BSDF_Diffuse(const RGBf & colorFactor = RGBf(1))
				: colorFactor(colorFactor), albedoTexture(nullptr) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2f & texcoord);

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD);

		private:
			const RGBf GetAlbedo(const Point2f & texcoord) const;

		public:
			RGBf colorFactor;
			Basic::Ptr<Basic::Image> albedoTexture;

		private:
			Basic::CosineWeightedHemisphereSampler3D sampler;
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_DIFFUSE_H_