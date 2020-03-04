#include <Engine/BSDF.h>

#include <Basic/UGM/Mat3x3.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

const Normalf BSDF::TangentSpaceNormalToWorld(const Normalf & worldTangent, const Normalf & worldNormal, const Normalf & tangentSpaceNormal) {
	const Normalf bitangent = worldTangent.Cross(worldNormal);
	Mat3f TBN(worldTangent, bitangent, worldNormal);
	return (TBN * tangentSpaceNormal).Normalize();
}
