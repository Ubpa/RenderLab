#include <CppUtil/Engine/BSDF.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace glm;

vec3 BSDF::LocalReflect(const vec3 & w) {
	return vec3(-w.x, -w.y, w.z);
}


bool BSDF::LocalRefract(const vec3& wo, vec3 & wi, float ior) {
	float inv = wo.z >= 0 ? 1.0f / ior : ior;

	float discriminant = 1 - (1 - wo.z * wo.z) * inv * inv;
	if (discriminant <= 0)
		return false;

	wi.x = -wo.x * inv;
	wi.y = -wo.y * inv;
	wi.z = (wo.z >= 0 ? -1 : 1) * sqrt(discriminant);
	normalize(wi);

	return true;
}
