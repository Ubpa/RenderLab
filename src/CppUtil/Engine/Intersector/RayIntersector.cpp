#include <CppUtil/Engine/RayIntersector.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/BVHNode.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Transform.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

RayIntersector::RayIntersector() {
	Reg<BVHAccel>();
	Reg<BVHNode<Element, BVHAccel>>();
	Reg<SObj>();
	Reg<Sphere>();
	Reg<Plane>();
	Reg<Triangle>();
}

void RayIntersector::Init(const Engine::Ray & ray) {
	this->ray = ray;

	rst.closestSObj = nullptr;
	rst.isIntersect = false;
}

bool RayIntersector::Intersect(const BBoxf & bbox) {
	float t0, t1;
	return Intersect(bbox, t0, t1);
}

bool RayIntersector::Intersect(const BBoxf & bbox, float & t0, float & t1) {
	const Pointf origin = ray.o;
	const Vectorf dir = ray.d;
	const Val3f invDir = ray.InvDir();
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

void RayIntersector::Visit(BVHAccel::Ptr bvhAccel) {
	rst.closestSObj = nullptr;
	bvhAccel->GetBVHRoot()->Accept(This());
	if (rst.closestSObj) {
		const auto & mat = bvhAccel->GetEleL2WMat(rst.closestSObj);
		rst.n = mat(rst.n).Norm();
		rst.tangent = mat(rst.tangent).Norm();
	}
}

void RayIntersector::Visit(BVHNode<Element, BVHAccel>::Ptr bvhNode) {
	if (bvhNode->IsLeaf()) {
		const auto origin = ray.o;
		const auto dir = ray.d;
		for (size_t i = 0; i < bvhNode->GetRange(); i++) {
			auto ele = bvhNode->GetObjs()[i + bvhNode->GetStart()];
			const auto & mat = bvhNode->GetHolder()->GetEleW2LMat(ele);
			mat.ApplyTo(ray);
			ele->Accept(This());
			if (rst.isIntersect)
				rst.closestSObj = bvhNode->GetHolder()->GetSObj(ele);

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
				if (t3 < ray.tMax)
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
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();

	if ((geometry == nullptr || !geometry->GetPrimitive()) && children.size() == 0)
		return;

	auto origSObj = rst.closestSObj;
	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform)
		cmptTransform->GetTransform().Inverse().ApplyTo(ray);

	if (geometry && geometry->GetPrimitive()) {
		geometry->GetPrimitive()->Accept(This());
		if (rst.isIntersect)
			rst.closestSObj = sobj;
	}

	for (auto child : children)
		child->Accept(This());

	if (cmptTransform) {
		cmptTransform->GetTransform().ApplyTo(ray);
		if (rst.closestSObj != origSObj) {
			cmptTransform->GetTransform().ApplyTo(rst.n).NormSelf();
			cmptTransform->GetTransform().ApplyTo(rst.tangent).NormSelf();
		}
	}
}

void RayIntersector::Visit(Sphere::Ptr sphere) {
	const auto & dir = ray.d;
	const auto & origin = ray.o;

	const Vectorf oc = origin;
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

	float t = - (b + sqrt_discriminant) * inv_a;
	if (t > tMax || t < tMin) {
		t = (-b + sqrt_discriminant) * inv_a;
		if (t > tMax || t < tMin) {
			rst.isIntersect = false;
			return;
		}
	}

	rst.isIntersect = true;
	ray.tMax = t;
	rst.n = ray(t);
	rst.texcoord = rst.n.ToTexcoord();
	rst.tangent = rst.n.ToTangent();
}

void RayIntersector::Visit(Plane::Ptr plane) {
	const auto & dir = ray.d;
	const auto & origin = ray.o;
	const float tMin = ray.tMin;
	const float tMax = ray.tMax;

	const float t = - origin.y / dir.y;
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
	ray.tMax = t;
	rst.n = Normalf(0, 1, 0);
	rst.texcoord = Point2f(pos.x + 0.5f, pos.z + 0.5f);
	rst.tangent = Normalf(1, 0, 0);
}

void RayIntersector::Visit(Triangle::Ptr triangle) {
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

	ray.tMax = t;

	const float w = 1 - u_plus_v;

	// normal
	const auto & normals = mesh->GetNormals();
	const auto & n1 = normals[idx1];
	const auto & n2 = normals[idx2];
	const auto & n3 = normals[idx3];

	rst.n = (w * n1 + u * n2 + v * n3).Norm();

	// texcoord
	const auto & texcoords = mesh->GetTexcoords();
	const auto & tc1 = texcoords[idx1];
	const auto & tc2 = texcoords[idx2];
	const auto & tc3 = texcoords[idx3];

	rst.texcoord.x = w * tc1.x + u * tc2.x + v * tc3.x;
	rst.texcoord.y = w * tc1.y + u * tc2.y + v * tc3.y;

	// tangent
	const auto & tangents = mesh->GetTangents();
	const auto & tg1 = tangents[idx1];
	const auto & tg2 = tangents[idx2];
	const auto & tg3 = tangents[idx3];

	rst.tangent = (w * tg1 + u * tg2 + v * tg3).Norm();
}
