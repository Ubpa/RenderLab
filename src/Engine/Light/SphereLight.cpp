#include <Engine/Light/SphereLight.h>

#include <Basic/Sampler/BasicSampler.h>

using namespace Ubpa;

using namespace std;

const rgbf SphereLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	if (p.cast_to<vecf3>().norm() < radius) {
		PD = 0;
		return rgbf(0.f);
	}

	// 暂时先简单地均匀采样
	auto normal = BasicSampler::UniformOnSphere().cast_to<normalf>();
	auto pos = (radius * normal).cast_to<pointf3>();
	auto d = pos - p;
	
	auto dist2 = d.dot(d);

	distToLight = std::sqrt(dist2);

	wi = (d / distToLight).cast_to<normalf>();

	float cosTheta = (-wi).dot(normal);
	float area = 4.f * PI<float> * radius * radius;
	PD = dist2 / (area * std::abs(cosTheta));

	return cosTheta < 0 ? rgbf(0.f) : Luminance();
}

float SphereLight::PDF(const pointf3 & p, const normalf & wi) const {
	// (p + t * wi)^2 = r^2
	// wi^2 * t^2 + 2*(p.wi) * t + p^2-r^2 = 0

	float a = wi.norm2();
	float b = p.cast_to<vecf3>().dot(wi.cast_to<vecf3>());
	float c = p.cast_to<vecf3>().norm2() - radius * radius;
	
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return 0;

	float t = -(b + discriminant) / a;
	if (t < 0)
		return 0;

	auto pos = p + t * wi.cast_to<vecf3>();
	auto dist2 = (p - pos).norm2();
	auto normal = pos.cast_to<vecf3>().normalize();
	float area = 4.f * PI<float> * radius * radius;
	float cosTheta = normal.dot(-wi.cast_to<vecf3>()); // positive
	return dist2 / (area * cosTheta);
}
