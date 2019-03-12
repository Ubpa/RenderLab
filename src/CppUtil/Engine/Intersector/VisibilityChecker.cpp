#include <CppUtil/Engine/VisibilityChecker.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Engine/BVHAccel.h>
#include <CppUtil/Engine/BVHNode.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

VisibilityChecker::VisibilityChecker()
	: ray(nullptr), rst(false)
{
	Reg<BVHAccel>();
	Reg<BVHNode<Element, BVHAccel>>();
	Reg<Sphere>();
	Reg<Plane>();
	Reg<Triangle>();
}

void VisibilityChecker::Init(Ray::Ptr ray, float tMax) {
	this->ray = ray;

	ray->SetTMax(tMax);

	rst.isIntersect = false;
}

bool VisibilityChecker::Intersect(const BBox & bbox) {
	float t0, t1;
	return Intersect(bbox, t0, t1);
}

bool VisibilityChecker::Intersect(const BBox & bbox, float & t0, float & t1) {
	const vec3 origin = ray->GetOrigin();
	const vec3 dir = ray->GetDir();
	const vec3 invDir = ray->GetInvDir();
	float tMin = ray->GetTMin();
	float tMax = ray->GetTMax();

	for (int i = 0; i < 3; i++) {
		float invD = invDir[i];
		float t0 = (bbox.minP[i] - origin[i]) * invD;
		float t1 = (bbox.maxP[i] - origin[i]) * invD;
		if (invD < 0.0f)
			std::swap(t0, t1);

		tMin = max(t0, tMin);
		tMax = min(t1, tMax);
		if (tMax <= tMin)
			return false;
	}

	t0 = tMin;
	t1 = tMax;

	return true;
}

void VisibilityChecker::Visit(BVHAccel::Ptr bvhAccel) {
	bvhAccel->GetBVHRoot()->Accept(This());
}

void VisibilityChecker::Visit(BVHNode<Element, BVHAccel>::Ptr bvhNode) {
	if (bvhNode->IsLeaf()) {
		const vec3 origin = ray->GetOrigin();
		const vec3 dir = ray->GetDir();
		for (size_t i = 0; i < bvhNode->GetRange(); i++) {
			auto ele = bvhNode->GetObjs()[i + bvhNode->GetStart()];
			const mat4 & mat = bvhNode->GetHolder()->GetEleW2LMat(ele);
			ray->Transform(mat);
			ele->Accept(This());
			if (rst.isIntersect)
				return;

			ray->SetOrigin(origin);
			ray->SetDir(dir);
		}
	}
	else {
		auto l = bvhNode->GetL();
		auto r = bvhNode->GetR();

		float t1, t2, t3, t4;
		bool leftBoxHit = Intersect(l->GetBBox(), t1, t2);
		bool rightBoxHit = Intersect(r->GetBBox(), t3, t4);
		if (leftBoxHit) {
			if (rightBoxHit) {
				auto first = (t1 <= t3) ? l : r;
				auto second = (t1 <= t3) ? r : l;
				Visit(first);
				if (rst.isIntersect)
					return;
				if (t3 < ray->GetTMax()) {
					Visit(second);
					if (rst.isIntersect)
						return;
				}
			}
			else {
				Visit(l);
				if (rst.isIntersect)
					return;
			}
		}
		else if (rightBoxHit) {
			Visit(r);
			if (rst.isIntersect)
				return;
		}
		else
			return;
	}
}

void VisibilityChecker::Visit(Sphere::Ptr sphere) {
	vec3 dir = ray->GetDir();
	vec3 origin = ray->GetOrigin();
	const vec3 & center = sphere->center;
	const float & radius = sphere->r;

	vec3 oc = origin - center;
	float a = dot(dir, dir);
	float b = dot(oc, dir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;

	if (discriminant < 0) {
		rst.isIntersect = false;
		return;
	}

	float tMin = ray->GetTMin();
	float tMax = ray->GetTMax();
	float sqrt_discriminant = sqrt(discriminant);
	float inv_a = 1.0f / a;

	float t = -(b + sqrt_discriminant) * inv_a;
	if (t > tMax || t < tMin) {
		t = (-b + sqrt_discriminant) * inv_a;
		if (t > tMax || t < tMin) {
			rst.isIntersect = false;
			return;
		}
	}

	rst.isIntersect = true;
}

void VisibilityChecker::Visit(Plane::Ptr plane) {
	vec3 dir = ray->GetDir();
	vec3 origin = ray->GetOrigin();
	float tMin = ray->GetTMin();
	float tMax = ray->GetTMax();

	float t = -origin.y / dir.y;
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
}

void VisibilityChecker::Visit(Triangle::Ptr triangle) {
	auto positions = triangle->GetMesh()->GetPositions();
	vec3 p1 = positions[triangle->idx[0]];
	vec3 p2 = positions[triangle->idx[1]];
	vec3 p3 = positions[triangle->idx[2]];

	const vec3 & dir = ray->GetDir();

	vec3 e1 = p2 - p1;
	vec3 e2 = p3 - p1;

	vec3 e1_x_d = cross(e1, dir);
	float denominator = dot(e1_x_d, e2);

	if (denominator == 0) {
		rst.isIntersect = false;
		return;
	}

	float inv_denominator = 1.0f / denominator;

	vec3 s = ray->GetOrigin() - p1;

	vec3 e2_x_s = cross(e2, s);
	float r1 = dot(e2_x_s, dir);
	float u = r1 * inv_denominator;
	if (u < 0 || u > 1) {
		rst.isIntersect = false;
		return;
	}

	float r2 = dot(e1_x_d, s);
	float v = r2 * inv_denominator;
	float u_plus_v = u + v;
	if (v < 0 || v > 1 || u_plus_v > 1) {
		rst.isIntersect = false;
		return;
	}

	float r3 = dot(e2_x_s, e1);
	float t = r3 * inv_denominator;
	if (t < ray->GetTMin() || t > ray->GetTMax()) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}
