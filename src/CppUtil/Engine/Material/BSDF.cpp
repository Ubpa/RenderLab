#include <CppUtil/Engine/BSDF.h>

#include <CppUtil/Basic/UGM/Mat3x3.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

bool BSDF::LocalRefract(const Normalf & wo, Normalf & wi, float ior) {
	float inv = wo.z >= 0 ? 1.0f / ior : ior;

	float discriminant = 1 - (1 - wo.z * wo.z) * inv * inv;
	if (discriminant <= 0)
		return false;

	wi.x = - wo.x * inv;
	wi.y = - wo.y * inv;
	wi.z = - Math::sgn(wo.z) * sqrt(discriminant);
	wi.NormalizeSelf();

	return true;
}

const Normalf BSDF::TangentSpaceNormalToWorld(const Normalf & worldTangent, const Normalf & worldNormal, const Normalf & tangentSpaceNormal) {
	const Normalf bitangent = worldTangent.Cross(worldNormal);
	Mat3f TBN(worldTangent, bitangent, worldNormal);
	return (TBN * tangentSpaceNormal).Normalize();
}
