#ifndef _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
#define _ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_

#include <CppUtil/Engine/BSDF.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {

		// Disney
		class BSDF_MetalWorkflow : public BSDF {
			ELE_SETUP(BSDF_MetalWorkflow)
		public:
			BSDF_MetalWorkflow(const RGBf & colorFactor = RGBf(1.f), float roughnessFactor = 1.f, float metallicFactor = 1.f)
				: colorFactor(colorFactor), roughnessFactor(roughnessFactor), metallicFactor(metallicFactor),
				albedoTexture(nullptr), metallicTexture(nullptr), aoTexture(nullptr) { }

			virtual const RGBf F(const Normalf & wo, const Normalf & wi, const Point2 & texcoord);

			// probability density function
			virtual float PDF(const Normalf & wo, const Normalf & wi, const Point2 & texcoord);

			// PD is probability density
			// return albedo
			virtual const RGBf Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD);

			virtual void ChangeNormal(const Point2 & texcoord, const Normalf & tangent, Normalf & normal) const;

		public:
			Basic::CPtr<Basic::Image> GetAlbedoTexture() const { return albedoTexture; }
			Basic::CPtr<Basic::Image> GetMetallicTexture() const { return metallicTexture; }
			Basic::CPtr<Basic::Image> GetRoughnessTexture() const { return roughnessTexture; }
			Basic::CPtr<Basic::Image> GetAOTexture() const { return aoTexture ; }
			Basic::CPtr<Basic::Image> GetNormalTexture() const { return normalTexture; }

			void SetAlbedoTexture(Basic::Ptr<Basic::Image> albedoTexture) {
				this->albedoTexture = albedoTexture;
			}

			void SetMetallicTexture(Basic::Ptr<Basic::Image> metallicTexture) {
				this->metallicTexture = metallicTexture;
			}

			void SetRoughnessTexture(Basic::Ptr<Basic::Image> roughnessTexture) {
				this->roughnessTexture = roughnessTexture;
			}

			void SetAOTexture(Basic::Ptr<Basic::Image> aoTexture) {
				this->aoTexture = aoTexture;
			}

			void SetNormalTexture(Basic::Ptr<Basic::Image> normalTexture) {
				this->normalTexture = normalTexture;
			}

		private:
			// Microfacet Specular BRDF
			static const RGBf MS_BRDF(const Normalf & wo, const Normalf & wi, const RGBf & albedo, float metallic, float roughness);
			// Microfacet Specular BRDF
			static const RGBf MS_BRDF(const Normalf & wo, const Normalf & wi, const RGBf & fr, const RGBf & albedo, float roughness);
			// Normal Distribution Function
			static float NDF(const Normalf & h, float roughness);
			// Fresnel
			static const RGBf Fr(const Normalf & wi, const Normalf & h, const RGBf & albedo, float metallic);
			// Geometric Attenuation
			static float G(const Normalf & wo, const Normalf & wi, float roughness);

			const RGBf GetAlbedo(const Point2 & texcoord) const;
			float GetMetallic(const Point2 & texcoord) const;
			float GetRoughness(const Point2 & texcoord) const;
			float GetAO(const Point2 & texcoord) const;

		public:
			RGBf colorFactor;
			Basic::Ptr<Basic::Image> albedoTexture;

			// 0--1
			float metallicFactor;
			Basic::Ptr<Basic::Image> metallicTexture;

			// 0--1
			float roughnessFactor;
			Basic::Ptr<Basic::Image> roughnessTexture;

			Basic::Ptr<Basic::Image> aoTexture;

			Basic::Ptr<Basic::Image> normalTexture;
		private:
		};
	}
}

#endif//!_ENGINE_MATERIAL_BSDF_METAL_WORKFLOW_H_
