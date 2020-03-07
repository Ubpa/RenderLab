#include <Basic/Sampler/BasicSampler.h>

#include <Basic/Math.h>
#include <UGM/vec.h>

using namespace Ubpa;

const pointf2 BasicSampler::UniformInSquare() {
	return { Math::Rand_F(), Math::Rand_F() };
}

const pointf2 BasicSampler::UniformInDisk() {
	auto u = UniformInSquare();

	// Map uniform random numbers to $[-1,1]^2$
	auto uOffset = (2.f * u.cast_to<vecf2>() - vecf2(1, 1)).cast_to<pointf2>();

	// Handle degeneracy at the origin
	if (uOffset[0] == 0 && uOffset[1] == 0)
		return pointf2(0, 0);

	// Apply concentric mapping to point
	float theta, r;
	if (std::abs(uOffset[0]) > std::abs(uOffset[1])) {
		r = uOffset[0];
		theta = Math::PI / 4.f * (uOffset[1] / uOffset[0]);
	}
	else {
		r = uOffset[1];
		theta = Math::PI / 2.f - Math::PI / 4.f * (uOffset[0] / uOffset[1]);
	}
	return pointf2(r * std::cos(theta), r * std::sin(theta));
}

const pointf2 BasicSampler::UniformOnDisk() {
	float theta = Math::Rand_F() * 2 * Math::PI;
	return { cos(theta), sin(theta) };
}

const vecf3 BasicSampler::UniformOnSphere() {
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

const vecf3 BasicSampler::UniformOnSphere(float & pd) {
	pd = PDofUniformOnSphere();
	return UniformOnSphere();
}

const vecf3 BasicSampler::CosOnHalfSphereMesh() {
	auto pInDiskMesh = UniformInDisk();
	float z = sqrt(1 - pInDiskMesh[0] * pInDiskMesh[0] - pInDiskMesh[1]*pInDiskMesh[1]);
	return { pInDiskMesh[0], pInDiskMesh[1], z };
}

const vecf3 BasicSampler::CosOnSphereMesh() {
	auto p = CosOnHalfSphereMesh();
	if (Math::Rand_F() < 0.5f)
		p[2] *= -1;

	return p;
}
