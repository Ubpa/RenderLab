#include <CppUtil/Basic/BasicSampler.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;

const Point2 BasicSampler::UniformInSquare() {
	return { Math::Rand_F(), Math::Rand_F() };
}

const Point2 BasicSampler::UniformInDisk() {
	auto u = UniformInSquare();

	// Map uniform random numbers to $[-1,1]^2$
	auto uOffset = static_cast<Point2>(2.f * Vec2(u) - Vec2(1, 1));

	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0)
		return Point2(0, 0);

	// Apply concentric mapping to point
	float theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = Math::PI / 4.f * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = Math::PI / 2.f - Math::PI / 4.f * (uOffset.x / uOffset.y);
	}
	return Point2f(r * std::cos(theta), r * std::sin(theta));
}

const Vec3 BasicSampler::UniformOnSphere() {
	auto Xi1 = Math::Rand_F();
	auto Xi2 = Math::Rand_F();

	auto phi = 2 * Math::PI * Xi2;
	auto t = 2 * sqrt(Xi1*(1 - Xi1));

	auto x = cos(phi) * t;
	auto y = sin(phi) * t;
	auto z = 1 - 2 * Xi1;

	return { x,y,z };
}

float BasicSampler::PDofUniformOnSphere() {
	return 1.f / (4.f*Math::PI);
}

const Vec3 BasicSampler::UniformOnSphere(float & pd) {
	pd = PDofUniformOnSphere();
	return UniformOnSphere();
}

const Vec3 BasicSampler::CosOnHalfSphere() {
	auto pInDisk = UniformInDisk();
	float z = sqrt(1 - pInDisk.x * pInDisk.x - pInDisk.y*pInDisk.y);
	return { pInDisk, z };
}

const Vec3 BasicSampler::CosOnSphere() {
	auto p = CosOnHalfSphere();
	if (Math::Rand_F() < 0.5f)
		p.z *= -1;

	return p;
}
