#include <CppUtil/Engine/CapsuleLight.h>

#include<CppUtil/Basic/BasicSampler.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

const RGBf CapsuleLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	// 暂时先简单地均匀采样
	float sphereArea = 4 * Math::PI * radius * radius;
	float cylinderArea = 2 * Math::PI * radius * height;
	float wholeArea = sphereArea + cylinderArea;

	Point3 pos;
	Normalf normal;
	if (Math::Rand_F() < sphereArea / wholeArea) {
		// Sphere
		pos = BasicSampler::UniformOnSphere();
		normal = pos;
		pos.y += pos.y >= 0 ? height / 2 : - height / 2;
	}
	else {
		// Cylinder
		auto posOnDisk = BasicSampler::UniformOnDisk();
		auto y = (Math::Rand_F() - 0.5f) * height;
		pos = Point3(posOnDisk[0], y, posOnDisk[1]);
		normal = Normalf(posOnDisk.x, 0, posOnDisk.y);
	}

	auto d = pos - p;

	auto dist2 = d.Dot(d);

	distToLight = sqrt(dist2);

	wi = d / distToLight;

	float cosTheta = (-wi).Dot(normal);
	PD = dist2 / (wholeArea * Math::Abs(cosTheta));

	return cosTheta < 0 ? RGBf(0) : intensity * color;
}

float CapsuleLight::PDF(const Point3 & p, const Normalf & wi) const {
	float halfH = height / 2;
	float sphereArea = 4 * Math::PI * radius * radius;
	float cylinderArea = 2 * Math::PI * radius * height;
	float wholeArea = sphereArea + cylinderArea;

	do { // 圆柱
		float a = wi.x * wi.x + wi.z * wi.z;
		float b = wi.x * p.x + wi.z * p.z;
		float c = p.x * p.x + p.z * p.z - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			return 0; // 柱形外

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		if (t < 0)
			return 0; // 后侧才相交

		auto pos = p + t * Vec3(wi);
		if (pos.y <= -halfH || pos.y >= halfH)
			break; // 不与圆柱相交

		float dist2 = p.Distance2With(pos);
		auto normal = Vec3(pos.x, 0, pos.y) / radius;

		float cosTheta = normal.Dot(p-pos) / sqrt(dist2); // positive
		return dist2 / (wholeArea * cosTheta);
	} while (false);

	//float a = wi.Dot(wi);
	float a = 1.f;

	do {// 上半球
		Point3 center(0, halfH, 0);
		auto oc = p - center;
		float b = wi.Dot(oc);
		float c = oc.Norm2() - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = p + t * Vec3(wi);
		if (t < 0)
			break;

		if (pos.y <= halfH)
			break;

		auto normal = (pos - center) / radius;
		float dist2 = pos.Distance2With(p);
		float cosTheta = normal.Dot(p - pos) / sqrt(dist2); // positive
		
		return dist2 / (wholeArea * cosTheta);
	} while (false);

	do {// 下半球
		Point3 center(0, -halfH, 0);
		auto oc = p - center;
		float b = wi.Dot(oc);
		float c = oc.Norm2() - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = p + t * Vec3(wi);
		if (t < 0)
			break;
		
		if (pos.y >= -halfH)
			break;

		auto normal = (pos - center) / radius;
		float dist2 = pos.Distance2With(p);
		float cosTheta = normal.Dot(p - pos) / sqrt(dist2); // positive

		return dist2 / (wholeArea * cosTheta);
	} while (false);

	return 0;
}
