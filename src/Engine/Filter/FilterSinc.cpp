#include <Engine/FilterSinc.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

float FilterSinc::WindowSinc(float x, float radius) const {
	x = std::abs(x);
	if (x > radius)
		return 0;
	const auto lanczos = Basic::Math::Sinc(x / tau);
	return Basic::Math::Sinc(x) * lanczos;
}
