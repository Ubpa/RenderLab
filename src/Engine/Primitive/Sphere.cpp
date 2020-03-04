#include <Engine/Sphere.h>

#include <Basic/UGM/Point2.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const Point2 Sphere::TexcoordOf(const Normalf & normal) {
	auto nDir = normal.Normalize();
	const float phi = std::atan2(-nDir.x, -nDir.z) + Math::PI;
	const float theta = acos(nDir.y);

	const auto u = phi / (2.f * Math::PI);
	const auto v = theta / Math::PI;
	return Point2(u, v);
}

const Normalf Sphere::TangentOf(const Normalf & normal) {
	auto nDir = normal.Normalize();
	const float phi = std::atan2(-nDir.x, -nDir.z) + Math::PI;

	return Normalf(cos(phi), 0, -sin(phi));
}

const Normalf Sphere::SphereCoord::ToDir() const {
	const auto sinTheta = sin(theta);

	float x = sinTheta * sin(phi);
	float y = cos(theta);
	float z = sinTheta * cos(phi);
	return { x,y,z };
}
