#include <CppUtil/Engine/DiskLight.h>

#include<CppUtil/Basic/BasicSampler.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil;

const RGBf DiskLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	if (p.y <= 0) {
		PD = 0;
		return RGBf(0);
	}

	auto Xi = BasicSampler::UniformInDisk();
	Point3 posOnLight(Xi.x * radius, 0, Xi.y * radius);

	const auto d = posOnLight - p;
	const float sqDist = d.Norm2();

	distToLight = sqrt(sqDist);
	wi = d / distToLight;

	float area = Math::PI * radius * radius;
	//float cosTheta = wi.y;
	PD = sqDist / (area * (-wi.y));
	return Luminance();
}

float DiskLight::PDF(const Point3 & p, const Normalf & wi) const {
	if (p.y <= 0 || wi.y >= 0)
		return 0;

	float t = -p.y / wi.y;
	auto pos = p + t * Vec3(wi);
	
	float r2 = radius * radius;
	if (Vec3f(pos).Norm2() >= r2)
		return 0;

	float dist2 = (p - pos).Norm2();
	return dist2 / (Math::PI * r2 * (-wi.y));
}
