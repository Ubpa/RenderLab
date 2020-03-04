#include <Engine/DirectionalLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf DirectionalLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	distToLight = FLT_MAX;
	wi = Normalf(0, 1, 0);
	PD = 1.f;
	return intensity * color;
}
