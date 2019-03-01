#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Engine/BVHNode.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Basic/Math.h>

#include <glm/geometric.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RayIntersector::RayIntersector(Ray::Ptr ray)
	: ray(ray) {
	Reg<BVHAccel>();
	Reg<BVHNode<Element, BVHAccel>>();
	Reg<SObj>();
	Reg<Sphere>();
	Reg<Plane>();
	Reg<Triangle>();
}

bool RayIntersector::Intersect(const BBox & bbox) {
	float t0, t1;
	return Intersect(bbox, t0, t1);
}

bool RayIntersector::Intersect(const BBox & bbox, float & t0, float & t1) {
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

void RayIntersector::Visit(BVHAccel::Ptr bvhAccel) {
	rst.closestSObj = nullptr;
	bvhAccel->GetBVHRoot()->Accept(This());
	if (rst.closestSObj) {
		auto mat = bvhAccel->GetNormL2WMat(rst.closestSObj);
		rst.n = mat * rst.n;
		rst.tangent = mat * rst.tangent;
	}
}

void RayIntersector::Visit(BVHNode<Element, BVHAccel>::Ptr bvhNode) {
	if (bvhNode->IsLeaf()) {
		const vec3 origin = ray->GetOrigin();
		const vec3 dir = ray->GetDir();
		for (size_t i = 0; i < bvhNode->GetRange(); i++) {
			auto ele = bvhNode->GetObjs()[i + bvhNode->GetStart()];
			const mat4 & mat = bvhNode->GetHolder()->GetEleW2LMat(ele);
			ray->Transform(mat);
			ele->Accept(This());
			if (rst.isIntersect)
				rst.closestSObj = bvhNode->GetHolder()->GetSObj(ele);

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
				if (t3 < ray->GetTMax())
					Visit(second);
			}
			else
				Visit(l);
		}
		else if (rightBoxHit)
			Visit(r);
		else
			return;
	}
}

void RayIntersector::Visit(SObj::Ptr sobj) {
	auto geometry = sobj->GetComponent<Geometry>();
	auto children = sobj->GetChildren();
	if (geometry == nullptr && children.size() == 0)
		return;

	auto origSObj = rst.closestSObj;
	auto transform = sobj->GetComponent<Transform>();
	if (transform)
		ray->Transform(transform->GetInv());

	if (geometry) {
		geometry->GetPrimitive()->Accept(This());
		if (rst.isIntersect)
			rst.closestSObj = sobj;
	}

	for (auto child : children)
		child->Accept(This());

	if (transform) {
		ray->Transform(transform->GetMat());
		if (rst.closestSObj != origSObj)
			rst.n = normalize(transform->GetNormMat() * rst.n);
	}
}

void RayIntersector::Visit(Sphere::Ptr sphere) {
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

	float t = - (b + sqrt_discriminant) * inv_a;
	if (t > tMax || t < tMin) {
		t = (-b + sqrt_discriminant) * inv_a;
		if (t > tMax || t < tMin) {
			rst.isIntersect = false;
			return;
		}
	}

	rst.isIntersect = true;
	ray->SetTMax(t);
	rst.n = (ray->At(t) - center) / radius;
	rst.texcoord = Math::SphereNormal2Texcoord(rst.n);
	rst.tangent = Math::SphereNormal2Tangent(rst.n);
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
	ray->SetTMax(t);
	rst.n = vec3(0, sign(origin.y), 0);
	rst.texcoord = vec2(pos.x, pos.z) + 0.5f;
	rst.tangent = vec3(1.f, 0, 0);
}

void RayIntersector::Visit(Triangle::Ptr triangle) {
	auto mesh = triangle->GetMesh();
	int idx1 = triangle->idx[0];
	int idx2 = triangle->idx[1];
	int idx3 = triangle->idx[2];

	auto const & positions = mesh->GetPositions();
	const vec3 & p1 = positions[idx1];
	const vec3 & p2 = positions[idx2];
	const vec3 & p3 = positions[idx3];

	const vec3 & dir = ray->GetDir();

	vec3 e1 = p2 - p1;
	vec3 e2 = p3 - p1;

	vec3 e1_x_d = cross(e1, dir);
	float denominator = dot(e1_x_d, e2);

	if (denominator == 0)
		return;

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

	ray->SetTMax(t);

	float w = 1 - u_plus_v;

	// normal
	auto const & normals = mesh->GetNormals();
	const vec3 & n1 = normals[idx1];
	const vec3 & n2 = normals[idx2];
	const vec3 & n3 = normals[idx3];

	rst.n = normalize(w * n1 + u * n2 + v * n3);

	// texcoord
	auto const & texcoords = mesh->GetTexcoords();
	const vec2 & tc1 = texcoords[idx1];
	const vec2 & tc2 = texcoords[idx2];
	const vec2 & tc3 = texcoords[idx3];

	rst.texcoord = w * tc1 + u * tc2 + v * tc3;

	// tangent
	auto const & tangents = mesh->GetTangents();
	const vec3 & tg1 = tangents[idx1];
	const vec3 & tg2 = tangents[idx2];
	const vec3 & tg3 = tangents[idx3];

	rst.tangent = normalize(w * tg1 + u * tg2 + v * tg3);
}
