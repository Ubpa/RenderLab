#include <Engine/PointLight.h>

using namespace Ubpa;

using namespace std;

float PointLight::Fwin(float d, float radius) {
	float ratio = d / radius;
	float ratio2 = ratio * ratio;
	float ratio4 = ratio2 * ratio2;
	float falloff = std::max(0.f, 1.f - ratio4);
	return falloff * falloff;
}

const rgbf PointLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	const auto d = p.cast_to<vecf3>();
	float dist2 = d.norm2();
	distToLight = sqrt(dist2);

	float falloff = Fwin(distToLight, radius);
	if (falloff == 0.f) {
		PD = 0.f;
		return rgbf(0.f);
	}

	wi = - (d / distToLight).cast_to<normalf>();
	PD = 1.0f;
	return intensity * color / dist2 * falloff;
}
