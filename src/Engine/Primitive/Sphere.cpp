#include <Engine/Primitive/Sphere.h>

#include <UGM/point.h>

using namespace Ubpa;

const pointf2 Sphere::TexcoordOf(const normalf & normal) {
	auto nDir = normal.normalize();
	const float phi = std::atan2(-nDir[0], -nDir[2]) + PI<float>;
	const float theta = acos(nDir[1]);

	const auto u = phi / (2.f * PI<float>);
	const auto v = theta / PI<float>;
	return pointf2(u, v);
}

const normalf Sphere::TangentOf(const normalf & normal) {
	auto nDir = normal.normalize();
	const float phi = std::atan2(-nDir[0], -nDir[2]) + PI<float>;

	return normalf(cos(phi), 0, -sin(phi));
}

const normalf Sphere::SphereCoord::ToDir() const {
	const auto sinTheta = sin(theta);

	float x = sinTheta * sin(phi);
	float y = cos(theta);
	float z = sinTheta * cos(phi);
	return { x,y,z };
}
