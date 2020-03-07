#pragma once

#include <Engine/Material/Material.h>
#include <Engine/Material/SurfCoord.h>

namespace Ubpa {
	class BSDF : public Material {
	protected:
		BSDF() = default;
		virtual ~BSDF() = default;

	public:
		virtual const rgbf F(const normalf& wo, const normalf& wi, const pointf2& texcoord) = 0;

		// probability density function
		virtual float PDF(const normalf& wo, const normalf& wi, const pointf2& texcoord) = 0;

		// PD is probability density
		// return albedo
		virtual const rgbf Sample_f(const normalf& wo, const pointf2& texcoord, normalf& wi, float& PD) = 0;

		virtual bool IsDelta() const { return false; }

		virtual void ChangeNormal(const pointf2& texcoord, const normalf& tangent, normalf& normal) const { return; };

		// Luminance
		virtual const rgbf Emission(const normalf& wo) const { return rgbf(0.f); }

	protected:
		static const normalf TangentSpaceNormalToWorld(const normalf& worldTangent, const normalf& worldNormal, const normalf& tangentSpaceNormal);
	};
}
