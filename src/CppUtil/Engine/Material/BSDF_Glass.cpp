#include <CppUtil/Engine/BSDF_Glass.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;

using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;


vec3 BSDF_Glass::Sample_f(const vec3 & wo, vec3 & wi, float & PD) {
	// PDF is delta

	if (!LocalRefract(wo, wi, ior)) {
		PD = 1.0f;
		wi = LocalReflect(wo);
		return 1.0f / abs(wi.z) * reflectance;
	}

	float cosTheta = wo.z >= 0 ? wo.z : wi.z;

	float R0 = pow((ior - 1) / (ior + 1), 2);

	float Fr = R0 + (1 - R0) * pow((1 - cosTheta), 5);

	if (Rand_F() < Fr) {
		PD = Fr;
		wi = LocalReflect(wo);
		return Fr / abs(wi.z) * reflectance;
	}

	PD = 1 - Fr;

	float iorRatio = wo.z > 0 ? 1.0f / ior : ior;
	float attenuation = iorRatio * iorRatio * (1 - Fr) / abs(wi.z);
	return attenuation * transmittance;
}
