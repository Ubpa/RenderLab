#include <CppUtil/Engine/Ray.h>

#include <glm/mat3x3.hpp>

using namespace CppUtil::Engine;
using namespace glm;

void Ray::Transform(const mat4 & mat) {
	dir = mat3(mat) * dir;
	invDir = vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
	origin = mat * vec4(origin, 1.0f);
}
