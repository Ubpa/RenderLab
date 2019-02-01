#include <CppUtil/Basic/UniformGridSampler2D.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Basic;
using namespace glm;

vec2 UniformGridSampler2D::GetSample() const {

	float x = Math::Rand_F();
	float y = Math::Rand_F();

	return vec2(x, y);
}
