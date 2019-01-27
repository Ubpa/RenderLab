#include <CppUtil/RTX/MoveSphere.h>

#include <CppUtil/RTX/TRay.h>

#include <CppUtil/Basic/Ptr.h>
#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

MoveSphere::MoveSphere(float t0, float t1, const vec3 & center0, const vec3 & center1, float radius, Material::CPtr material)
	: radius(radius), Hitable(material), center0(center0), center1(center1), t0(t0), t1(t1) {
	vec3 minP = min(center0 - vec3(radius), center1 - vec3(radius));
	vec3 maxP = max(center0 + vec3(radius), center1 + vec3(radius));
	box = AABB(minP, maxP);
}

HitRst MoveSphere::RayIn(Ray::Ptr & ray) const {
	TRay::Ptr tRay = std::dynamic_pointer_cast<TRay>(ray);
	vec3 center;
	if (tRay != NULL)
		center = mix(center0, center1, tRay->GetTime());
	else
		center = center0;

	vec3 oc = ray->GetOrigin() - center;
	float a = dot(ray->GetDir(), ray->GetDir());
	float b = dot(oc, ray->GetDir());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant <= 0)
		return HitRst::InValid;

	float t = (-b - sqrt(discriminant)) / a;
	if (t > ray->GetTMax() || t < Ray::tMin) {
		t = (-b + sqrt(discriminant)) / a;
		if (t > ray->GetTMax() || t < Ray::tMin)
			return HitRst::InValid;
	}

	ray->SetTMax(t);

	HitRst hitRst(true);
	vec3 pos = ray->At(t);
	vec3 normal = (pos - center) / radius;
	vec2 uv = Math::Sphere2UV(normal);
	hitRst.record = HitRecord(ray, pos, normal, uv[0], uv[1]);
	hitRst.material = GetMat();
	hitRst.isMatCoverable = IsMatCoverable();

	return hitRst;
}