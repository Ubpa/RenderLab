#include <Engine/BSDF.h>

#include <UGM/mat.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

const Ubpa::normalf BSDF::TangentSpaceNormalToWorld(const Ubpa::normalf & worldTangent, const Ubpa::normalf & worldNormal, const Ubpa::normalf & tangentSpaceNormal) {
	const Ubpa::normalf bitangent = worldTangent.cross(worldNormal);
	Ubpa::matf3 TBN(worldTangent.cast_to<Ubpa::vecf3>(), bitangent.cast_to<Ubpa::vecf3>(), worldNormal.cast_to<Ubpa::vecf3>());
	return (TBN * tangentSpaceNormal.cast_to<Ubpa::vecf3>()).normalize().cast_to<Ubpa::normalf>();
}
