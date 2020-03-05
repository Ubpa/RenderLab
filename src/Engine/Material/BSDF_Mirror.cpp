#include <Engine/BSDF_Mirror.h>

#include <Engine/SurfCoord.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic::Math;


const Ubpa::rgbf BSDF_Mirror::Sample_f(const Ubpa::normalf & wo, const Ubpa::pointf2 & texcoord, Ubpa::normalf & wi, float & PD) {
	wi = SurfCoord::Reflect(wo);

	// delta
	PD = 1.0f;

	return 1.0f / abs(wi[2]) * reflectance;
}
