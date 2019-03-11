#include <CppUtil/Engine/BSDF.h>

#include <glm/geometric.hpp>
#include <glm/mat3x3.hpp>

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

vec3 BSDF::TangentSpaceNormalToWorld(const vec3 & worldTangent, const vec3 worldNormal, const vec3 & tangentSpaceNormal) {
	const vec3 bitangent = cross(worldTangent, worldNormal);
	mat3 TBN(worldTangent, bitangent, worldNormal);
	return normalize(TBN * tangentSpaceNormal);
}
