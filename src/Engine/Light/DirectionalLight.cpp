#include <Engine/DirectionalLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const Ubpa::rgbf DirectionalLight::Sample_L(const Ubpa::pointf3 & p, Ubpa::normalf & wi, float & distToLight, float & PD) const {
	distToLight = FLT_MAX;
	wi = Ubpa::normalf(0, 1, 0);
	PD = 1.f;
	return intensity * color;
}
