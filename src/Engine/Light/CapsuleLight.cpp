#include <Engine/Light/CapsuleLight.h>

#include <Basic/Sampler/BasicSampler.h>
#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

const rgbf CapsuleLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	// 暂时先简单地均匀采样
	float sphereArea = 4 * PI<float> * radius * radius;
	float cylinderArea = 2 * PI<float> * radius * height;
	float wholeArea = sphereArea + cylinderArea;

	pointf3 pos;
	normalf normal;
	if (Math::Rand_F() < sphereArea / wholeArea) {
		// Sphere
		pos = BasicSampler::UniformOnSphere().cast_to<pointf3>();
		normal = pos.cast_to<normalf>();
		pos[1] += pos[1] >= 0 ? height / 2 : - height / 2;
	}
	else {
		// Cylinder
		auto posOnDisk = BasicSampler::UniformOnDisk();
		auto y = (Math::Rand_F() - 0.5f) * height;
		pos = pointf3(posOnDisk[0], y, posOnDisk[1]);
		normal = normalf(posOnDisk[0], 0, posOnDisk[1]);
	}

	auto d = pos - p;

	auto dist2 = d.dot(d);

	distToLight = sqrt(dist2);

	wi = (d / distToLight).cast_to<normalf>();

	float cosTheta = (-wi).dot(normal);
	PD = dist2 / (wholeArea * std::abs(cosTheta));

	return cosTheta < 0 ? rgbf(0.f) : Luminance();
}

float CapsuleLight::PDF(const pointf3 & p, const normalf & wi) const {
	float halfH = height / 2;
	float sphereArea = 4 * PI<float> * radius * radius;
	float cylinderArea = 2 * PI<float> * radius * height;
	float wholeArea = sphereArea + cylinderArea;

	do { // 圆柱
		float a = wi[0] * wi[0] + wi[2] * wi[2];
		float b = wi[0] * p[0] + wi[2] * p[2];
		float c = p[0] * p[0] + p[2] * p[2] - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			return 0; // 柱形外

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		if (t < 0)
			return 0; // 后侧才相交

		auto pos = p + t * wi.cast_to<vecf3>();
		if (pos[1] <= -halfH || pos[1] >= halfH)
			break; // 不与圆柱相交

		float dist2 = pointf3::distance2(p, pos);
		auto normal = vecf3(pos[0], 0, pos[1]) / radius;

		float cosTheta = normal.dot(p-pos) / sqrt(dist2); // positive
		return dist2 / (wholeArea * cosTheta);
	} while (false);

	//float a = wi.dot(wi);
	float a = 1.f;

	do {// 上半球
		pointf3 center(0, halfH, 0);
		vecf3 oc = p - center;
		float b = wi.cast_to<vecf3>().dot(oc);
		float c = oc.norm2() - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = p + t * wi.cast_to<vecf3>();
		if (t < 0)
			break;

		if (pos[1] <= halfH)
			break;

		auto normal = (pos - center) / radius;
		float dist2 = pointf3::distance2(p, pos);
		float cosTheta = normal.dot(p - pos) / sqrt(dist2); // positive
		
		return dist2 / (wholeArea * cosTheta);
	} while (false);

	do {// 下半球
		pointf3 center(0, -halfH, 0);
		auto oc = p - center;
		float b = wi.cast_to<vecf3>().dot(oc);
		float c = oc.norm2() - radius * radius;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = p + t * wi.cast_to<vecf3>();
		if (t < 0)
			break;
		
		if (pos[1] >= -halfH)
			break;

		auto normal = (pos - center) / radius;
		float dist2 = pointf3::distance2(pos, p);
		float cosTheta = normal.dot(p - pos) / sqrt(dist2); // positive

		return dist2 / (wholeArea * cosTheta);
	} while (false);

	return 0;
}
