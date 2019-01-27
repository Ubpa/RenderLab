#include <CppUtil/RTX/Transform.h>

#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

#include <bitset>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

Transform::Transform(const mat4 & transform, Hitable::CPtr hitable, Material::CPtr material)
	: transform(transform),
	inverseTransform(inverse(transform)),
	normalTransform(transpose(inverse(mat3(transform)))),
	hitable(hitable),
	Hitable(material) {
	if (hitable == NULL) {
		box = AABB::InValid;
		return;
	}

	auto childBox = hitable->GetBoundingBox();
	if (!childBox.IsValid()) {
		box = AABB::InValid;
		return;
	}

	vec3 srcP[2] = { childBox.GetMinP(), childBox.GetMaxP() };
	vector<vec3> tfmCornerPs;
	for (size_t i = 0; i < 8; i++) {
		bitset<3> binVal(i);
		vec3 cornerP = vec3(srcP[binVal[2]].x, srcP[binVal[1]].y, srcP[binVal[0]].z);
		vec4 tfmCornerPQ = transform * vec4(cornerP, 1);
		tfmCornerPs.push_back(vec3(tfmCornerPQ) / tfmCornerPQ.w);
	}

	vec3 minP = Math::min(tfmCornerPs);
	vec3 maxP = Math::max(tfmCornerPs);
	box = AABB(minP, maxP);
}

HitRst Transform::RayIn(Ray::Ptr & ray) const {
	if (hitable == NULL)
		return HitRst::InValid;

	ray->Transform(inverseTransform);

	auto hitRst = hitable->RayIn(ray);

	if (hitRst.hit) {
		if (hitRst.isMatCoverable && GetMat() != NULL) {
			hitRst.material = GetMat();
			hitRst.isMatCoverable = IsMatCoverable();
		}

		hitRst.record.vertex.Transform(transform, normalTransform);
	}

	ray->Transform(transform);

	return hitRst;
}