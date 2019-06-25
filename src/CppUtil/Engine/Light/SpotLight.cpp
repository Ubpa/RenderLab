#include <CppUtil/Engine/SpotLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

float SpotLight::Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = Math::Max(0.f, 1.f - ratio4);
	return falloff * falloff;
}

const RGBf SpotLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	const auto d = Vec3(p);
	float dist2 = d.Norm2();
	distToLight = sqrt(dist2);

	float falloff = Fwin(distToLight, radius);
	if (Math::IsZero(falloff)) {
		PD = 0;
		return Vec3f(0);
	}

	wi = -d / distToLight;
	PD = 1.0f;
	return Falloff(wi) * intensity * color / dist2 * falloff;
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
