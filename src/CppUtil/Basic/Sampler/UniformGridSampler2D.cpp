#include <CppUtil/Basic/UniformGridSampler2D.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;

const Point2f UniformGridSampler2D::GetSample() const {

	float x = Math::Rand_F();
	float y = Math::Rand_F();

	return Point2f(x, y);
}
