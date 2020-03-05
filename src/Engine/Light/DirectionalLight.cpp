#include <Engine/DirectionalLight.h>

using namespace Ubpa;

using namespace std;

const rgbf DirectionalLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	distToLight = FLT_MAX;
	wi = normalf(0, 1, 0);
	PD = 1.f;
	return intensity * color;
}
