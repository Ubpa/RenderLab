#include <CppUtil/RTX/ConstTexture.h>

using namespace RTX;
using namespace glm;

ConstTexture::ConstTexture(const glm::rgb & color)
	: color(color) { }

rgb ConstTexture::Value(float u, float v, const vec3 & p) const {
	return color;
}