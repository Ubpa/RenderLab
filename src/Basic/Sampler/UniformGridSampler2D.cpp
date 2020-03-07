#include <Basic/Sampler/UniformGridSampler2D.h>

#include <Basic/Math.h>

using namespace Ubpa;

const valf2 UniformGridSampler2D::GetSample() const {

	float x = Math::Rand_F();
	float y = Math::Rand_F();

	return valf2(x, y);
}
