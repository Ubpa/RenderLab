#include <CppUtil/Engine/AreaLight.h>

#include <CppUtil/Basic/UGM/Point2.h>

using namespace CppUtil;
using namespace CppUtil::Engine;

const RGBf AreaLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	if (p.y <= 0) {
		PD = 0;
		return RGBf(0);
	}

	auto sample = Point2(sampler.GetSample()) - Point2(0.5f, 0.5f);
	Point3 posOnLight(sample.x*width / 2, 0, sample.y*height / 2);

	const auto d = posOnLight - p;
	const float sqDist = d.Norm2();

	distToLight = sqrt(sqDist);
	wi = d / distToLight;

	const float area = width * height;
	//float cosTheta = wi.y;
	PD = sqDist / (area * (-wi.y));
	return Luminance();
}

float AreaLight::PDF(const Point3 & p, const Normalf & wi) const {
	Point3 posOnLight;
	if (!Hit(p, wi, posOnLight))
		return 0;
	
	const auto d = posOnLight - p;
	const float sqDist = d.Norm2();
	const float dist = sqrt(sqDist);
	const float cosTheta = d.y / dist;
	const float area = width * height;
	return sqDist / (area * (-cosTheta));
}

bool AreaLight::Hit(const Point3 & p, const Vec3 & dirToLight, Point3 & hitPos) const {
	if (p.y <= 0 || dirToLight.y > 0)
		return false;

	float t = - p.y / dirToLight.y;
	float x = p.x + dirToLight.x * t;
	if (x >= width / 2 || x <= -width / 2)
		return false;

	float z = p.z + dirToLight.z * t;
	if (z >= height / 2 || z <= -height / 2)
		return false;

	hitPos = Point3(x, 0, z);

	return true;
}
