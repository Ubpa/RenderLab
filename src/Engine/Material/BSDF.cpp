#include <Engine/Material/BSDF.h>

#include <UGM/mat.h>

#include <Basic/Math.h>

using namespace Ubpa;

const normalf BSDF::TangentSpaceNormalToWorld(const normalf & worldTangent, const normalf & worldNormal, const normalf & tangentSpaceNormal) {
	const normalf bitangent = worldTangent.cross(worldNormal);
	matf3 TBN(worldTangent.cast_to<vecf3>(), bitangent.cast_to<vecf3>(), worldNormal.cast_to<vecf3>());
	return (TBN * tangentSpaceNormal.cast_to<vecf3>()).normalize().cast_to<normalf>();
}
