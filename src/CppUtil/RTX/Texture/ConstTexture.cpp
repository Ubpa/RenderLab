#include <CppUtil/RTX/ConstTexture.h>

using namespace RTX;
using namespace glm;

ConstTexture::ConstTexture(const glm::vec3 & color)
	: color(color) { }

vec3 ConstTexture::Value(float u, float v, const vec3 & p) const {
	return color;
}