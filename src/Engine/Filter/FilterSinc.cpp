#include <Engine/Filter/FilterSinc.h>

#include <Basic/Math.h>

using namespace Ubpa;

float FilterSinc::WindowSinc(float x, float radius) const {
	x = std::abs(x);
	if (x > radius)
		return 0;
	const auto lanczos = Math::Sinc(x / tau);
	return Math::Sinc(x) * lanczos;
}
