#include <CppUtil/Engine/BSDF.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace glm;

bool BSDF::LocalRefract(const vec3& wo, vec3 & wi, float ior) {
	float inv = wo.z >= 0 ? 1.0f / ior : ior;

	float discriminant = 1 - (1 - wo.z * wo.z) * inv * inv;
	if (discriminant <= 0)
		return false;

	wi.x = - wo.x * inv;
	wi.y = - wo.y * inv;
	wi.z = - sign(wo.z) * sqrt(discriminant);
	normalize(wi);

	return true;
}
