#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RayIntersector::RayIntersector() {
	Reg<Sphere>();
	Reg<Plane>();
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
		rst.isIntersect = false;
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
			rst.isIntersect = false;
			return;
		}
	}

	rst.isIntersect = true;
	rst.t = t;
	rst.n = (ray->At(t) - center) / radius;
}

void RayIntersector::Visit(Plane::Ptr plane) {
	vec3 dir = ray->GetDir();
	vec3 origin = ray->GetOrigin();
	float tMin = ray->GetTMin();
	float tMax = ray->GetTMax();

	float t = - origin.y / dir.y;
	if (t<tMin || t>tMax) {
		rst.isIntersect = false;
		return;
	}

	vec3 pos = ray->At(t);
	if (pos.x<-0.5 || pos.x>0.5 || pos.z<-0.5 || pos.z>0.5) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
	rst.t = t;
	rst.n = vec3(0, sign(origin.y), 0);
}
