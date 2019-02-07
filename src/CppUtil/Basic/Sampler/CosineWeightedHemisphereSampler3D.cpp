#include <CppUtil/Basic/CosineWeightedHemisphereSampler3D.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Basic;
using namespace glm;

vec3 CosineWeightedHemisphereSampler3D::GetSample() const {
	float pd;
	return GetSample(pd);
}

vec3 CosineWeightedHemisphereSampler3D::GetSample(float & pd) const {
	float Xi1 = Math::Rand_F();
	float Xi2 = Math::Rand_F();

	float sinTheta = sqrt(Xi1);
	float cosTheta = sqrt(1 - Xi1);

	float phi = 2.0f * Math::PI * Xi2;

	float xs = sinTheta * cos(phi);
	float ys = sinTheta * sin(phi);
	float zs = cosTheta;

	// pdf 是 p(w)，而不是 p(theta, phi)
	pd = cosTheta / Math::PI;

	return vec3(xs, ys, zs);
}
