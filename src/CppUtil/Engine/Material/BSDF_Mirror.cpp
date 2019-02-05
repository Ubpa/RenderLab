#include <CppUtil/Engine/BSDF_Mirror.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;

using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;
using namespace glm;


vec3 BSDF_Mirror::Sample_f(const vec3 & wo, vec3 & wi, float & PD) {
	wi = LocalReflect(wo);

	// delta
	PD = 1.0f;

	return 1.0f / abs(wi.z) * albedo;
}
