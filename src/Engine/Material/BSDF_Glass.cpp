#include <Engine/BSDF_Glass.h>

#include <Engine/SurfCoord.h>

using namespace Ubpa;

using namespace Ubpa::Math;

const rgbf BSDF_Glass::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & PD) {
	// PDF is delta

	if (!SurfCoord::Refract(wo, wi, ior)) {
		// 全反射
		PD = 1.0f;
		wi = SurfCoord::Reflect(wo);
		return 1.0f / abs(wi[2]) * reflectance;
	}

	// 用空气中的角作为 cos theta
	float cosTheta = wo[2] >= 0 ? wo[2] : wi[2];

	float R0 = pow((ior - 1) / (ior + 1), 2);

	float Fr = R0 + (1 - R0) * pow((1 - cosTheta), 5);

	if (Rand_F() < Fr) {
		// 菲涅尔 反射

		PD = Fr;
		wi = SurfCoord::Reflect(wo);
		return Fr / abs(wi[2]) * reflectance;
	}

	PD = 1 - Fr;

	float iorRatio = wo[2] > 0 ? 1.0f / ior : ior;
	float attenuation = iorRatio * iorRatio * (1 - Fr) / abs(wi[2]);
	return attenuation * transmittance;
}
