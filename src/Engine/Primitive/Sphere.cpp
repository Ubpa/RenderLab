#include <Engine/Sphere.h>

#include <UGM/point.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const Ubpa::pointf2 Sphere::TexcoordOf(const Ubpa::normalf & normal) {
	auto nDir = normal.normalize();
	const float phi = std::atan2(-nDir[0], -nDir[2]) + Ubpa::PI<float>;
	const float theta = acos(nDir[1]);

	const auto u = phi / (2.f * Ubpa::PI<float>);
	const auto v = theta / Ubpa::PI<float>;
	return Ubpa::pointf2(u, v);
}

const Ubpa::normalf Sphere::TangentOf(const Ubpa::normalf & normal) {
	auto nDir = normal.normalize();
	const float phi = std::atan2(-nDir[0], -nDir[2]) + Ubpa::PI<float>;

	return Ubpa::normalf(cos(phi), 0, -sin(phi));
}

const Ubpa::normalf Sphere::SphereCoord::ToDir() const {
	const auto sinTheta = sin(theta);

	float x = sinTheta * sin(phi);
	float y = cos(theta);
	float z = sinTheta * cos(phi);
	return { x,y,z };
}
