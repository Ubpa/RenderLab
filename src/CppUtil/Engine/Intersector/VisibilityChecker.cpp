#include <CppUtil/Engine/VisibilityChecker.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Engine/BVHAccel.h>
#include <CppUtil/Engine/BVHNode.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

VisibilityChecker::VisibilityChecker()
	: rst(false)
{
	Reg<BVHAccel>();
	Reg<BVHNode<Element, BVHAccel>>();
	Reg<Sphere>();
	Reg<Plane>();
	Reg<Triangle>();
}

void VisibilityChecker::Init(const Engine::Ray & ray, const float tMax) {
	this->ray = ray;

	this->ray.tMax = tMax;

	rst.isIntersect = false;
}

bool VisibilityChecker::Intersect(const BBoxf & bbox) {
	float t0, t1;
	return Intersect(bbox, t0, t1);
}

bool VisibilityChecker::Intersect(const BBoxf & bbox, float & t0, float & t1) {
	const auto & origin = ray.o;
	const auto & dir = ray.d;
	const auto invDir = ray.InvDir();
	float tMin = ray.tMin;
	float tMax = ray.tMax;

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
		const auto origin = ray.o;
		const auto dir = ray.d;
		for (size_t i = 0; i < bvhNode->GetRange(); i++) {
			auto ele = bvhNode->GetObjs()[i + bvhNode->GetStart()];
			const auto & mat = bvhNode->GetHolder()->GetEleW2LMat(ele);
			mat.ApplyTo(ray);
			ele->Accept(This());
			if (rst.isIntersect)
				return;

			ray.o = origin;
			ray.d = dir;
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
				if (t3 < ray.tMax) {
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
	const auto & dir = ray.d;
	const auto & origin = ray.o;

	const Vec3 oc = origin;
	const float a = dir.Dot(dir);
	const float b = oc.Dot(dir);
	const float c = oc.Dot(oc) - 1;
	const float discriminant = b * b - a * c;

	if (discriminant < 0) {
		rst.isIntersect = false;
		return;
	}

	const float tMin = ray.tMin;
	const float tMax = ray.tMax;
	const float sqrt_discriminant = sqrt(discriminant);
	const float inv_a = 1.0f / a;

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
	const auto & dir = ray.d;
	const auto & origin = ray.o;
	const float tMin = ray.tMin;
	const float tMax = ray.tMax;

	const float t = -origin.y / dir.y;
	if (t<tMin || t>tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray(t);
	if (pos.x<-0.5 || pos.x>0.5 || pos.z<-0.5 || pos.z>0.5) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}

void VisibilityChecker::Visit(Triangle::Ptr triangle) {
	const auto mesh = triangle->GetMesh();
	const int idx1 = triangle->idx[0];
	const int idx2 = triangle->idx[1];
	const int idx3 = triangle->idx[2];

	const auto & positions = mesh->GetPositions();
	const auto & p1 = positions[idx1];
	const auto & p2 = positions[idx2];
	const auto & p3 = positions[idx3];

	const auto & dir = ray.d;

	const auto e1 = p2 - p1;
	const auto e2 = p3 - p1;

	const auto e1_x_d = e1.Cross(dir);
	const float denominator = e1_x_d.Dot(e2);

	if (denominator == 0)
		return;

	const float inv_denominator = 1.0f / denominator;

	const auto s = ray.o - p1;

	const auto e2_x_s = e2.Cross(s);
	const float r1 = e2_x_s.Dot(dir);
	const float u = r1 * inv_denominator;
	if (u < 0 || u > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r2 = e1_x_d.Dot(s);
	const float v = r2 * inv_denominator;
	const float u_plus_v = u + v;
	if (v < 0 || v > 1 || u_plus_v > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r3 = e2_x_s.Dot(e1);
	const float t = r3 * inv_denominator;

	if (t < ray.tMin || t > ray.tMax) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}
