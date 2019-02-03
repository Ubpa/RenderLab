#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Sphere.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RayIntersector::RayIntersector(CppUtil::Basic::CPtr<Ray> ray)
	: ray(ray) {
	Reg<Sphere>();
}

bool RayIntersector::Visit(CppUtil::Basic::Ptr<Sphere> sphere) {
	const vec3 dir = ray->GetDir();
	const vec3 origin = ray->GetOrigin();
	const vec3 center = sphere->GetCenter();
	const float radius = sphere->GetR();

	vec3 oc = origin - center;
	float a = dot(dir, dir);
	float b = dot(oc, dir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant <= 0) {
		rst.Clear();
		return true;
	}

	const float tMin = ray->GetTMin();
	float t = (-b - sqrt(discriminant)) / a;
	if (t > ray->GetTMax() || t < tMin) {
		t = (-b + sqrt(discriminant)) / a;
		if (t > ray->GetTMax() || t < tMin) {
			rst.Clear();
			return true;
		}
	}

	rst.isIntersect = true;
	rst.t = t;

	return true;
}
