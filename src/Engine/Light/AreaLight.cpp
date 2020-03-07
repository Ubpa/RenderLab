#include <Engine/Light/AreaLight.h>

#include <UGM/point.h>

using namespace Ubpa;

const rgbf AreaLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	if (p[1] <= 0) {
		PD = 0;
		return rgbf(0.f);
	}

	auto sample = sampler.GetSample().cast_to<pointf2>() - pointf2(0.5f, 0.5f);
	pointf3 posOnLight(sample[0]*width / 2, 0, sample[1]*height / 2);

	const auto d = posOnLight - p;
	const float sqDist = d.norm2();

	distToLight = sqrt(sqDist);
	wi = (d / distToLight).cast_to<normalf>();

	const float area = width * height;
	//float cosTheta = wi[1];
	PD = sqDist / (area * (-wi[1]));
	return Luminance();
}

float AreaLight::PDF(const pointf3 & p, const normalf & wi) const {
	pointf3 posOnLight;
	if (!Hit(p, wi.cast_to<vecf3>(), posOnLight))
		return 0;
	
	const auto d = posOnLight - p;
	const float sqDist = d.norm2();
	const float dist = sqrt(sqDist);
	const float cosTheta = d[1] / dist;
	const float area = width * height;
	return sqDist / (area * (-cosTheta));
}

bool AreaLight::Hit(const pointf3 & p, const vecf3 & dirToLight, pointf3 & hitPos) const {
	if (p[1] <= 0 || dirToLight[1] > 0)
		return false;

	float t = - p[1] / dirToLight[1];
	float x = p[0] + dirToLight[0] * t;
	if (x >= width / 2 || x <= -width / 2)
		return false;

	float z = p[2] + dirToLight[2] * t;
	if (z >= height / 2 || z <= -height / 2)
		return false;

	hitPos = pointf3(x, 0, z);

	return true;
}
