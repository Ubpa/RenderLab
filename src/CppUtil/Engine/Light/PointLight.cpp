#include <CppUtil/Engine/PointLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float PointLight::Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = Math::Max(0.f, 1.f - ratio4);
	return falloff * falloff;
}

const RGBf PointLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	const auto d = Vec3(p);
	float dist2 = d.Norm2();
	distToLight = sqrt(dist2);

	float falloff = Fwin(distToLight, radius);
	if (Math::IsZero(falloff)) {
		PD = 0.f;
		return Vec3f(0);
	}

	wi = - d / distToLight;
	PD = 1.0f;
	return intensity * color / dist2 * falloff;
}
