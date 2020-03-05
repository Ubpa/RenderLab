#include <Engine/DiskLight.h>

#include <Basic/BasicSampler.h>

using namespace Ubpa;

const rgbf DiskLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	if (p[1] <= 0) {
		PD = 0;
		return rgbf(0.f);
	}

	auto Xi = BasicSampler::UniformInDisk();
	pointf3 posOnLight(Xi[0] * radius, 0, Xi[1] * radius);

	const auto d = posOnLight - p;
	const float sqDist = d.norm2();

	distToLight = sqrt(sqDist);
	wi = (d / distToLight).cast_to<normalf>();

	float area = PI<float> * radius * radius;
	//float cosTheta = wi[1];
	PD = sqDist / (area * (-wi[1]));
	return Luminance();
}

float DiskLight::PDF(const pointf3 & p, const normalf & wi) const {
	if (p[1] <= 0 || wi[1] >= 0)
		return 0;

	float t = -p[1] / wi[1];
	auto pos = p + t * wi.cast_to<vecf3>();
	
	float r2 = radius * radius;
	if (pos.cast_to<vecf3>().norm2() >= r2)
		return 0;

	float dist2 = (p - pos).norm2();
	return dist2 / (PI<float> * r2 * (-wi[1]));
}
