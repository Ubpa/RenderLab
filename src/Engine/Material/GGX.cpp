#include <Engine/Material/GGX.h>

#include <Engine/Material/SurfCoord.h>

using namespace Ubpa;

float GGX::D(const normalf & wh) const {
	if (SurfCoord::CosTheta(wh) < 0)
		return 0.f;

	float alpha2 = alpha * alpha;
	float cos2Theta = SurfCoord::Cos2Theta(wh);
	
	float t = (alpha2 - 1) * cos2Theta + 1;

	return alpha2 / (PI<float> * t * t);
}

float GGX::Lambda(const normalf & w) const {
	auto absTanTheta = std::abs(SurfCoord::TanTheta(w));
	if (std::isnan(absTanTheta))
		return 98e8f;

	auto alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);

	return (-1.f + std::sqrt(1.f + alpha2Tan2Theta)) / 2.f;
}

const normalf GGX::Sample_wh() const {
	// sample
	const float Xi1 = Math::Rand_F();
	const float Xi2 = Math::Rand_F();

	// theta
	const auto cos2Theta = (1 - Xi1) / ((alpha*alpha - 1)*Xi1 + 1);
	const auto cosTheta = sqrt(cos2Theta);
	const auto sinTheta = sqrt(1 - cos2Theta);

	// phi
	const auto phi = 2 * PI<float> * Xi2;

	return SurfCoord::SphericalDirection(sinTheta, cosTheta, phi);
}
