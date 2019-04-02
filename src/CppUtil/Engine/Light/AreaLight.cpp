#include <CppUtil/Engine/AreaLight.h>

using namespace CppUtil;
using namespace CppUtil::Engine;

const RGBf AreaLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	if (p.y >= 0) {
		PD = 0;
		return RGBf(0);
	}

	Point2 sample = Point2(sampler.GetSample()) - Point2(0.5f, 0.5f);
	Point3 posOnLight(sample.x*width / 2, 0, sample.y*height / 2);

	const auto d = posOnLight - p;
	const float sqDist = d.Length2();
	const float dist = sqrt(sqDist);

	wi = d / dist;
	distToLight = dist;
	const float area = width * height;
	// float cosTheta = wi.y;
	PD = sqDist / (area * wi.y);
	return intensity * color;
}

float AreaLight::PDF(const Point3 & p, const Normalf & wi) const {
	Point3 posOnLight;
	if (!Hit(p, wi, posOnLight))
		return 0;
	
	const auto d = posOnLight - p;
	const float sqDist = d.Length2();
	const float dist = sqrt(sqDist);
	const float cosTheta = d.y / dist;
	const float area = width * height;
	return sqDist / (area * cosTheta);
}

const RGBf AreaLight::GetL(const Point3 & p, const Vec3 & dirToLight, float & distToLight) {
	Point3 posOnLight;
	if (!Hit(p, dirToLight, posOnLight))
		return RGBf(0);

	const auto d = posOnLight - p;
	distToLight = d.Length();

	return intensity * color;
}

const RGBf AreaLight::GetMaxL(const Point3 & p) const {
	return p.y < 0 ? intensity * color : RGBf(0);
}

bool AreaLight::Hit(const Point3 & p, const Vec3 & dirToLight, Point3 & hitPos) const {
	if (p.y >= 0 || dirToLight.y < 0)
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
