#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Sphere.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RayIntersector::RayIntersector() {
	Reg<Sphere>();
}

void RayIntersector::Visit(CppUtil::Basic::Ptr<Sphere> sphere) {
	vec3 dir = ray->GetDir();
	vec3 origin = ray->GetOrigin();
	vec3 center = sphere->GetCenter();
	float radius = sphere->GetR();

	vec3 oc = origin - center;
	float a = dot(dir, dir);
	float b = dot(oc, dir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant <= 0) {
		rst.Clear();
		return;
	}

	float tMin = ray->GetTMin();
	float tMax = ray->GetTMax();
	float sqrt_discriminant = sqrt(discriminant);
	float inv_a = 1.0f / a;

	float t = - (b + sqrt_discriminant) * inv_a;
	if (t > tMax || t < tMin) {
		t = (-b + sqrt_discriminant) * inv_a;
		if (t > tMax || t < tMin) {
			rst.Clear();
			return;
		}
	}

	rst.isIntersect = true;
	rst.t = t;

	return;
}
