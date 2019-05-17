#include <CppUtil/Engine/SpotLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf SpotLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	const auto d = Vec3(p);
	float dist2 = d.Norm2();
	distToLight = sqrt(dist2);

	wi = -d / distToLight;
	PD = 1.0f;
	return Falloff(wi) * intensity * color / (1.0f + linear * distToLight + quadratic * dist2);
}

float SpotLight::Falloff(const Normalf & wi) const {
	float rAngle = Math::Radians(angle);
	float cosAngle = cos(rAngle / 2);
	float cosFalloffAngle = cos(rAngle * fullRatio / 2);
	float cosTheta = wi.y;
	if (cosTheta < cosAngle) return 0;
	if (cosTheta > cosFalloffAngle) return 1;

	float delta = (cosTheta - cosAngle) /
		(cosAngle - cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
