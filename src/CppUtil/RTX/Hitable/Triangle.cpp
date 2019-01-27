#include <CppUtil/RTX/Triangle.h>

#include <CppUtil/RTX/Ray.h>

#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;


Triangle::Triangle(const Vertex & A, const Vertex & B, const Vertex & C, Material::CPtr material)
	: A(A), B(B), C(C), Hitable(material) {
	vec3 minP = min(min(A.pos, B.pos), C.pos);
	vec3 maxP = max(max(A.pos, B.pos), C.pos);
	for (size_t i = 0; i < 3; i++) {
		if (minP[i] == maxP[i]) {
			minP[i] -= 0.001f;
			maxP[i] += 0.001f;
		}
	}
	box = AABB(minP, maxP);
}

HitRst Triangle::RayIn(CppUtil::Basic::Ptr<Ray> & ray) const {
	vec4 abgt = Math::Intersect_RayTri(ray->GetOrigin(), ray->GetDir(), A.pos, B.pos, C.pos);
	if (abgt == vec4(0)
		|| abgt[0] < 0 || abgt[0] > 1
		|| abgt[1] < 0 || abgt[1] > 1
		|| abgt[2] < 0 || abgt[2] > 1
		|| abgt[3] < Ray::tMin || abgt[3] > ray->GetTMax())
		return HitRst::InValid;

	HitRst hitRst(true);
	hitRst.record.vertex = Vertex::Interpolate_Tri(vec3(abgt[0], abgt[1], abgt[2]), A, B, C);
	//hitRst.hitable = this;
	hitRst.record.ray = ray;
	hitRst.material = GetMat();
	hitRst.isMatCoverable = IsMatCoverable();
	ray->SetTMax(abgt[3]);
	return hitRst;
}