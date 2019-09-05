#include <CppUtil/Engine/SphereLight.h>

#include<CppUtil/Basic/BasicSampler.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf SphereLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	if (Vec3f(p).Norm() < radius) {
		PD = 0;
		return RGBf(0);
	}

	// 暂时先简单地均匀采样
	auto normal = BasicSampler::UniformOnSphere();
	auto pos = radius * normal;
	auto d = pos - p;
	
	auto dist2 = d.Dot(d);

	distToLight = sqrt(dist2);

	wi = d / distToLight;

	float cosTheta = (-wi).Dot(normal);
	float area = 4.f * Math::PI * radius * radius;
	PD = dist2 / (area * Math::Abs(cosTheta));

	return cosTheta < 0 ? RGBf(0) : Luminance();
}

float SphereLight::PDF(const Point3 & p, const Normalf & wi) const {
	// (p + t * wi)^2 = r^2
	// wi^2 * t^2 + 2*(p.wi) * t + p^2-r^2 = 0

	float a = wi.Norm2();
	float b = Vec3f(p).Dot(wi);
	float c = Vec3f(p).Norm2() - radius * radius;
	
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return 0;

	float t = -(b + discriminant) / a;
	if (t < 0)
		return 0;

	auto pos = p + t * Vec3(wi);
	auto dist2 = (p - pos).Norm2();
	auto normal = Vec3f(pos).Normalize();
	float area = 4.f * Math::PI * radius * radius;
	float cosTheta = normal.Dot(-wi); // positive
	return dist2 / (area * cosTheta);
}
