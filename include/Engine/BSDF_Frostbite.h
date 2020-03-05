#ifndef _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTBITE_H_
#define _CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTBITE_H_

#include <Engine/BSDF.h>
#include <Engine/GGX.h>

namespace CppUtil {
	namespace Engine {
		class BSDF_Frostbite : public BSDF {
		public:
			BSDF_Frostbite(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), metallicFactor(metallicFactor),
				albedoTexture(nullptr), metallicTexture(nullptr), aoTexture(nullptr), roughnessTexture(nullptr), normalTexture(nullptr) { }

		public:
			static const Basic::Ptr<BSDF_Frostbite> New(const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f) {
				return Basic::New<BSDF_Frostbite>(colorFactor, roughnessFactor, metallicFactor);
			}

		protected:
			virtual ~BSDF_Frostbite() = default;

		public:
			virtual const Ubpa::rgbf F(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// probability density function
			virtual float PDF(const Ubpa::normalf & wo, const Ubpa::normalf & wi, const Ubpa::pointf2 & texcoord) override;

			// PD is probability density
			// return albedo
			virtual const Ubpa::rgbf Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) override;

			virtual void ChangeNormal(const Ubpa::pointf2 & texcoord, const Ubpa::normalf & tangent, Ubpa::normalf & normal) const override;

		private:
			// Fresnel
			static const Ubpa::rgbf Fr(const Ubpa::normalf & w, const Ubpa::normalf & h, const Ubpa::rgbf & albedo, float metallic);

			const Ubpa::rgbf GetAlbedo(const Ubpa::pointf2 & texcoord) const;
			float GetMetallic(const Ubpa::pointf2 & texcoord) const;
			float GetRoughness(const Ubpa::pointf2 & texcoord) const;
			float GetAO(const Ubpa::pointf2 & texcoord) const;

			static const float Fr_DisneyDiffuse(const Ubpa::normalf & wo, const Ubpa::normalf & wi, float linearRoughness);

		public:
			GGX ggx;

			Ubpa::rgbf colorFactor;
			Basic::Ptr<Basic::Image> albedoTexture;

			// 0--1
			float metallicFactor;
			Basic::Ptr<Basic::Image> metallicTexture;

			// 0--1
			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture; // 只用与实时渲染

			Basic::Ptr<Basic::Image> normalTexture;
		};
	}
}


#endif //!_CPPUTIL_ENGINE_MATERIAL_BSDF_FROSTBITE_H_