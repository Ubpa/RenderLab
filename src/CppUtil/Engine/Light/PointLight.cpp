#include <CppUtil/Engine/PointLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf PointLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	const auto d = Vec3(p);
	float dist2 = d.Norm2();
	distToLight = sqrt(dist2);
	wi = - d / distToLight;
	PD = 1.0f + linear * distToLight + quadratic * dist2;
	return intensity * color;
}
