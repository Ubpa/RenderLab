#pragma once

#include <Engine/BSDF.h>
#include <Engine/GGX.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_FrostedGlass : public BSDF {
		public:
			BSDF_FrostedGlass(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float roughnessFactor = 1.f, float ior = 1.5f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), ior(ior),
				colorTexture(nullptr),
				roughnessTexture(nullptr),
				aoTexture(nullptr),
				normalTexture(nullptr) { }

		public:
			static const Basic::Ptr<BSDF_FrostedGlass> New(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float roughnessFactor = 1.f, float ior = 1.5f) {
				return Basic::New<BSDF_FrostedGlass>(colorFactor, roughnessFactor, ior);
			}

		protected:
			virtual ~BSDF_FrostedGlass() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

			virtual void ChangeNormal(const Ubpa::pointf2 & texcoord, const Ubpa::normalf & tangent, Ubpa::normalf & normal) const override;

		private:
			static float Fr(const Ubpa::normalf & v, const Ubpa::normalf & h, float ior);

		private:
			const Ubpa::rgbf GetColor(const Ubpa::pointf2 & texcoord) const;
			float GetRoughness(const Ubpa::pointf2 & texcoord) const;
			float GetAO(const Ubpa::pointf2 & texcoord) const;

		public:
			GGX ggx;

			Ubpa::rgbf colorFactor;
			Basic::Ptr<Basic::Image> colorTexture;

			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture;
			Basic::Ptr<Basic::Image> normalTexture;

			float ior;
		};
	}
}
