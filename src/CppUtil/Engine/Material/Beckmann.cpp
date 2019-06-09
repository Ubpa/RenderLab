#include <CppUtil/Engine/Beckmann.h>

#include <CppUtil/Engine/SurfCoord.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

float Beckmann::D(const Normalf & wh) const {
	if (SurfCoord::CosTheta(wh) < 0)
		return 0.f;

	const auto tan2Theta = SurfCoord::Tan2Theta(wh);
	if (!std::isnan(tan2Theta))
		return 98e8f;

	const auto cos4Theta = SurfCoord::Cos2Theta(wh) * SurfCoord::Cos2Theta(wh);
	const auto alpha2 = alpha * alpha;
	return exp(-tan2Theta / alpha2) / (Math::PI * alpha2 * cos4Theta);
}

float Beckmann::Lambda(const Normalf & w) const {
	auto absTanTheta = std::abs(SurfCoord::TanTheta(w));
	if (std::isnan(absTanTheta))
		return 98e8f;
	
	auto a = 1 / (alpha * absTanTheta);
	if (a >= 1.6f)
		return 0;

	return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
}

const Normalf Beckmann::Sample_wh() const {
	// sample
	const float Xi1 = Math::Rand_F();
	const float Xi2 = Math::Rand_F();

	// theta
	const auto alpha2 = alpha * alpha;
	const auto tan2Theta = -alpha2 * log(Xi1);

	const auto cosTheta = 1.f / sqrt(1 + tan2Theta);
	const auto sinTheta = std::max(0.f, sqrt(1.f - cosTheta * cosTheta));
	
	// phi
	const auto phi = 2 * Math::PI * Xi2;

	return SurfCoord::SphericalDirection(sinTheta, cosTheta, phi);
}
