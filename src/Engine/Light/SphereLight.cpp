#include <Engine/SphereLight.h>

#include <Basic/BasicSampler.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const Ubpa::rgbf SphereLight::Sample_L(const Ubpa::pointf3 & p, Ubpa::normalf & wi, float & distToLight, float & PD) const {
	if (p.cast_to<Ubpa::vecf3>().norm() < radius) {
		PD = 0;
		return Ubpa::rgbf(0.f);
	}

	// 暂时先简单地均匀采样
	auto normal = BasicSampler::UniformOnSphere().cast_to<Ubpa::normalf>();
	auto pos = (radius * normal).cast_to<Ubpa::pointf3>();
	auto d = pos - p;
	
	auto dist2 = d.dot(d);

	distToLight = std::sqrt(dist2);

	wi = (d / distToLight).cast_to<Ubpa::normalf>();

	float cosTheta = (-wi).dot(normal);
	float area = 4.f * Ubpa::PI<float> * radius * radius;
	PD = dist2 / (area * std::abs(cosTheta));

	return cosTheta < 0 ? Ubpa::rgbf(0.f) : Luminance();
}

float SphereLight::PDF(const Ubpa::pointf3 & p, const Ubpa::normalf & wi) const {
	// (p + t * wi)^2 = r^2
	// wi^2 * t^2 + 2*(p.wi) * t + p^2-r^2 = 0

	float a = wi.norm2();
	float b = p.cast_to<Ubpa::vecf3>().dot(wi.cast_to<Ubpa::vecf3>());
	float c = p.cast_to<Ubpa::vecf3>().norm2() - radius * radius;
	
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return 0;

	float t = -(b + discriminant) / a;
	if (t < 0)
		return 0;

	auto pos = p + t * wi.cast_to<Ubpa::vecf3>();
	auto dist2 = (p - pos).norm2();
	auto normal = pos.cast_to<Ubpa::vecf3>().normalize();
	float area = 4.f * Ubpa::PI<float> * radius * radius;
	float cosTheta = normal.dot(-wi.cast_to<Ubpa::vecf3>()); // positive
	return dist2 / (area * cosTheta);
}
