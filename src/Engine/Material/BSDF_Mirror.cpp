#include <Engine/BSDF_Mirror.h>

#include <Engine/SurfCoord.h>

using namespace Ubpa;

using namespace Ubpa::Math;

const rgbf BSDF_Mirror::Sample_f(const normalf & wo, const pointf2 & texcoord, normalf & wi, float & PD) {
	wi = SurfCoord::Reflect(wo);

	// delta
	PD = 1.0f;

	return 1.0f / abs(wi[2]) * reflectance;
}
