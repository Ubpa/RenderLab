#include <Basic/UniformGridSampler2D.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;

const Ubpa::valf2 UniformGridSampler2D::GetSample() const {

	float x = Math::Rand_F();
	float y = Math::Rand_F();

	return Ubpa::valf2(x, y);
}
