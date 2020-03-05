#pragma once

#include <Engine/BSDF.h>

#include <Basic/CosineWeightedHemisphereSampler3D.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Diffuse final : public BSDF {
		public:
			BSDF_Diffuse(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f))
				: colorFactor(colorFactor), albedoTexture(nullptr) { }

		public:
			static const Basic::Ptr<BSDF_Diffuse> New(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f)) {
				return Basic::New<BSDF_Diffuse>(colorFactor);
			}

		protected:
			virtual ~BSDF_Diffuse() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

		private:
			const Ubpa::rgbf GetAlbedo(const Ubpa::pointf2 & texcoord) const;

		public:
			Ubpa::rgbf colorFactor;
			Basic::Ptr<Basic::Image> albedoTexture;

		private:
			Basic::CosineWeightedHemisphereSampler3D sampler;
		};
	}
}
