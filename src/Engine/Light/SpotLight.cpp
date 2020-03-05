#include <Engine/SpotLight.h>

using namespace Ubpa;

using namespace std;

float SpotLight::Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = std::max(0.f, 1.f - ratio4);
	return falloff * falloff;
}

const rgbf SpotLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	const auto d = p.cast_to<vecf3>();
	float dist2 = d.norm2();
	distToLight = sqrt(dist2);

	float falloff = Fwin(distToLight, radius);
	if (falloff == 0.f) {
		PD = 0;
		return rgbf(0.f);
	}

	wi = -(d / distToLight).cast_to<normalf>();
	PD = 1.0f;
	return Falloff(wi) * intensity * color / dist2 * falloff;
}

float SpotLight::Falloff(const normalf & wi) const {
	float rAngle = to_radian(angle);
	float cosAngle = cos(rAngle / 2);
	float cosFalloffAngle = cos(rAngle * fullRatio / 2);
	float cosTheta = wi[1];
	if (cosTheta < cosAngle) return 0;
	if (cosTheta > cosFalloffAngle) return 1;

	float delta = (cosTheta - cosAngle) /
		(cosAngle - cosFalloffAngle);

	return (delta * delta) * (delta * delta);
}
