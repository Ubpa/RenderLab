#include <CppUtil/Engine/BSDF_Mirror.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;


const RGBf BSDF_Mirror::Sample_f(const Normalf & wo, const Point2f & texcoord, Normalf & wi, float & PD) {
	wi = LocalReflect(wo);

	// delta
	PD = 1.0f;

	return 1.0f / abs(wi.z) * reflectance;
}
