#include <CppUtil/Basic/UniformGridSampler2D.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;

const Val2 UniformGridSampler2D::GetSample() const {

	float x = Math::Rand_F();
	float y = Math::Rand_F();

	return Val2(x, y);
}
