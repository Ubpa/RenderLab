#include <CppUtil/Engine/GGX.h>

#include <CppUtil/Engine/SurfCoord.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

float GGX::D(const Normalf & wh) const {
	if (SurfCoord::CosTheta(wh) < 0)
		return 0.f;

	const auto tan2Theta = SurfCoord::Tan2Theta(wh);
	if (std::isinf(tan2Theta))
		return 0.f;

	const auto cos4Theta = SurfCoord::Cos2Theta(wh) * SurfCoord::Cos2Theta(wh);

	//const auto alpha2 = alpha * alpha;
	//auto e = tan2Theta / alpha2;
	//return 1 / (Math::PI * alpha2 * cos4Theta * (1 + e) * (1 + e));

	float t = alpha + tan2Theta / alpha;
	return 1 / (Math::PI * cos4Theta * t * t);
}

float GGX::Lambda(const Normalf & w) const {
	auto absTanTheta = std::abs(SurfCoord::TanTheta(w));
	if (std::isinf(absTanTheta))
		return 0.f;

	auto alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);

	return (-1.f + std::sqrt(1.f + alpha2Tan2Theta)) / 2.f;
}

const Normalf GGX::Sample_wh() const {
	// sample
	const float Xi1 = Math::Rand_F();
	const float Xi2 = Math::Rand_F();

	// theta
	const auto alpha2 = alpha * alpha;
	const auto tan2Theta = alpha2 * Xi1 / (1 - Xi1);

	const auto cosTheta = 1.f / sqrt(1 + tan2Theta);
	const auto sinTheta = std::max(0.f, sqrt(1.f - cosTheta * cosTheta));

	// phi
	const auto phi = 2 * Math::PI * Xi2;

	return SurfCoord::SphericalDirection(sinTheta, cosTheta, phi);
}
