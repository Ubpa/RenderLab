#include <Engine/BSDF_Mirror.h>

#include <Engine/SurfCoord.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;


const RGBf BSDF_Mirror::Sample_f(const Normalf & wo, const Point2 & texcoord, Normalf & wi, float & PD) {
	wi = SurfCoord::Reflect(wo);

	// delta
	PD = 1.0f;

	return 1.0f / abs(wi.z) * reflectance;
}
