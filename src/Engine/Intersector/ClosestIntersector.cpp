#include <Engine/Intersector/ClosestIntersector.h>

#include <Engine/Scene/SObj.h>
#include <Engine/Viewer/Ray.h>

#include <Engine/Viewer/BVHAccel.h>

#include <Engine/Scene/CmptGeometry.h>
#include <Engine/Scene/CmptTransform.h>

#include <Engine/Primitive/Sphere.h>
#include <Engine/Primitive/Plane.h>
#include <Engine/Primitive/Triangle.h>
#include <Engine/Primitive/TriMesh.h>
#include <Engine/Primitive/Disk.h>
#include <Engine/Primitive/Capsule.h>

#include <Basic/Math.h>
#include <UGM/transform.h>

#include <stack>

using namespace Ubpa;

using namespace std;

ClosestIntersector::ClosestIntersector() {
	Regist<Sphere, Plane, Triangle, TriMesh, Disk, Capsule>();
}

void ClosestIntersector::Init(Ray * ray) {
	this->ray = ray;

	rst.closestSObj = nullptr;
	rst.isIntersect = false;
}

bool ClosestIntersector::Intersect(const bboxf3 & bbox, const valf3 & invDir) const {
	const auto & origin = ray->o;

	float tMin = ray->tMin;
	float tMax = ray->tMax;

	for (int i = 0; i < 3; i++) {
		float invD = invDir[i];
		float t0 = (bbox.minP()[i] - origin[i]) * invD;
		float t1 = (bbox.maxP()[i] - origin[i]) * invD;
		if (invD < 0.0f)
			swap(t0, t1);

		tMin = max(t0, tMin);
		tMax = min(t1, tMax);
		if (tMax < tMin)
			return false;
	}

	return true;
}

void ClosestIntersector::Visit(Ptr<BVHAccel> bvhAccel) {
	const auto origin = ray->o;
	const auto dir = ray->d;
	const auto invDir = ray->InvDir();
	const bool dirIsNeg[3] = { invDir[0] < 0,invDir[1] < 0,invDir[2] < 0 };

	stack<int> nodeIdxStack;
	nodeIdxStack.push(0);
	while (!nodeIdxStack.empty()) {
		const auto nodeIdx = nodeIdxStack.top();
		nodeIdxStack.pop();
		const auto & node = bvhAccel->GetBVHNode(nodeIdx);

		if (!Intersect(node.GetBox(), invDir))
			continue;
		
		if (node.IsLeaf()) {
			for (auto shapeIdx : node.ShapesIdx()) {
				auto shape = bvhAccel->GetShape(shapeIdx);

				auto bray = bvhAccel->GetShapeW2LMat(shape) * (*ray);
				ray->o = bray.o;
				ray->d = bray.d;
				Visit(shape);
				ray->o = origin;
				ray->d = dir;

				if (rst.isIntersect) {
					rst.closestSObj = bvhAccel->GetSObj(shape);
					rst.isIntersect = false;
				}
			}
		}
		else {
			const auto firstChildIdx = BVHAccel::LinearBVHNode::FirstChildIdx(nodeIdx);
			const auto secondChildIdx = node.GetSecondChildIdx();
			if (dirIsNeg[node.GetAxis()]) {
				nodeIdxStack.push(firstChildIdx);
				nodeIdxStack.push(secondChildIdx);
			}
			else {
				nodeIdxStack.push(secondChildIdx);
				nodeIdxStack.push(firstChildIdx);
			}
		}
	}

	if (rst.closestSObj) {
		const Ptr<Shape> shape = rst.closestSObj->GetComponent<CmptGeometry>()->primitive;
		const auto l2w = bvhAccel->GetShapeW2LMat(shape).inverse();
		rst.n = (l2w * rst.n).normalize();
		rst.tangent = (l2w * rst.tangent).normalize();
	}
}

void ClosestIntersector::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();

	if ((geometry == nullptr || !geometry->primitive) && children.size() == 0)
		return;

	auto origSObj = rst.closestSObj;
	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform) {
		auto bray = cmptTransform->GetTransform().inverse() * (*ray);
		ray->o = bray.o;
		ray->d = bray.d;
	}

	if (geometry && geometry->primitive) {
		Visit(geometry->primitive);
		if (rst.isIntersect)
			rst.closestSObj = sobj;
	}

	for (auto child : children)
		Visit(child);

	if (cmptTransform) {
		auto& tsfm = cmptTransform->GetTransform();
		auto bray = tsfm.inverse() * (*ray);
		ray->o = bray.o;
		ray->d = bray.d;
		if (rst.closestSObj != origSObj) {
			rst.n = (tsfm * rst.n).normalize();
			rst.tangent = (tsfm * rst.tangent).normalize();
		}
	}
}

void ClosestIntersector::ImplVisit(Ptr<Sphere> sphere) {
	const auto & dir = ray->d;
	const auto & origin = ray->o;

	const vecf3 oc = origin.cast_to<vecf3>();
	const float a = dir.dot(dir);
	const float b = oc.dot(dir);
	const float c = oc.dot(oc) - 1;
	const float discriminant = b * b - a * c;

	if (discriminant < 0) {
		rst.isIntersect = false;
		return;
	}

	const float tMin = ray->tMin;
	const float tMax = ray->tMax;
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
	ray->tMax = t;
	rst.n = ray->at(t).cast_to<normalf>();
	rst.texcoord = Sphere::TexcoordOf(rst.n);
	rst.tangent = Sphere::TangentOf(rst.n);
}

void ClosestIntersector::ImplVisit(Ptr<Plane> plane) {
	const float t = -ray->o[1] / ray->d[1];
	if (t<ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray->at(t);
	if (pos[0]<-0.5 || pos[0]>0.5 || pos[2]<-0.5 || pos[2]>0.5) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
	ray->tMax = t;
	//rst.n = normalf(0, -Math::sgn(ray->d[1]), 0);
	rst.n = normalf(0, 1, 0);
	rst.texcoord = pointf2(pos[0] + 0.5f, pos[2] + 0.5f);
	rst.tangent = normalf(1, 0, 0);
}

void ClosestIntersector::ImplVisit(Ptr<Triangle> triangle) {
	const auto mesh = triangle->GetMesh();
	const int idx1 = triangle->idx[0];
	const int idx2 = triangle->idx[1];
	const int idx3 = triangle->idx[2];

	const auto & positions = mesh->GetPositions();
	const auto & p1 = positions[idx1];
	const auto & p2 = positions[idx2];
	const auto & p3 = positions[idx3];

	const auto & dir = ray->d;

	const auto e1 = p2 - p1;
	const auto e2 = p3 - p1;

	const auto e1_x_d = e1.cross(dir);
	const float denominator = e1_x_d.dot(e2);

	if (denominator == 0) {
		rst.isIntersect = false;
		return;
	}

	const float inv_denominator = 1.0f / denominator;

	const auto s = ray->o - p1;

	const auto e2_x_s = e2.cross(s);
	const float r1 = e2_x_s.dot(dir);
	const float u = r1 * inv_denominator;
	if (u < 0 || u > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r2 = e1_x_d.dot(s);
	const float v = r2 * inv_denominator;
	if (v < 0 || v > 1) {
		rst.isIntersect = false;
		return;
	}

	const float u_plus_v = u + v;
	if (u_plus_v > 1) {
		rst.isIntersect = false;
		return;
	}

	const float r3 = e2_x_s.dot(e1);
	const float t = r3 * inv_denominator;

	if (t < ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;

	ray->tMax = t;

	const float w = 1 - u_plus_v;

	// normal
	const auto & normals = mesh->GetNormals();
	const auto & n1 = normals[idx1];
	const auto & n2 = normals[idx2];
	const auto & n3 = normals[idx3];

	rst.n = (w * n1 + u * n2 + v * n3).normalize();

	// texcoord
	const auto & texcoords = mesh->GetTexcoords();
	const auto & tc1 = texcoords[idx1];
	const auto & tc2 = texcoords[idx2];
	const auto & tc3 = texcoords[idx3];

	rst.texcoord[0] = w * tc1[0] + u * tc2[0] + v * tc3[0];
	rst.texcoord[1] = w * tc1[1] + u * tc2[1] + v * tc3[1];

	// tangent
	const auto & tangents = mesh->GetTangents();
	const auto & tg1 = tangents[idx1];
	const auto & tg2 = tangents[idx2];
	const auto & tg3 = tangents[idx3];

	rst.tangent = (w * tg1 + u * tg2 + v * tg3).normalize();

	rst.triangle = triangle;
	if (w < u) {
		if (u < v)
			rst.idx = idx3;
		else
			rst.idx = idx2;
	}
	else {
		if (w > v)
			rst.idx = idx1;
		else
			rst.idx = idx3;
	}
}

void ClosestIntersector::ImplVisit(Ptr<TriMesh> mesh) {
	const auto visitor = This();
	for (auto triangle : mesh->GetTriangles()) {
		Visit(triangle);
		if (rst.isIntersect) {
			return;
		}
	}
}

void ClosestIntersector::ImplVisit(Ptr<Disk> disk) {
	const float t = -ray->o[1] / ray->d[1];
	if (t < ray->tMin || t > ray->tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray->at(t);
	if (pos.cast_to<vecf3>().norm2() >= 1.f) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
	ray->tMax = t;
	//rst.n = normalf(0, -Math::sgn(ray->d[1]), 0);
	rst.n = normalf(0, 1, 0);
	rst.texcoord = pointf2((1+pos[0])/2, (1+pos[2])/2);
	rst.tangent = normalf(1, 0, 0);
}

void ClosestIntersector::ImplVisit(Ptr<Capsule> capsule) {
	const auto & o = ray->o;
	const auto & d = ray->d;

	float halfH = capsule->height / 2;

	do{ // ‘≤÷˘
		float a = d[0] * d[0] + d[2] * d[2];
		float b = d[0] * o[0] + d[2] * o[2];
		float c = o[0] * o[0] + o[2] * o[2] - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0) {
			rst.isIntersect = false;
			return;
		}

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		if (t < ray->tMin || t > ray->tMax) {
			t = (sqrtDiscriminant - b) / a;
			if (t < ray->tMin || t > ray->tMax)
				break;
		}

		auto pos = ray->at(t);
		if (pos[1] <= -halfH || pos[1] >= halfH)
			break;

		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = normalf(pos[0], 0, pos[2]);
		rst.texcoord = Sphere::TexcoordOf(pos.cast_to<normalf>());
		rst.tangent = Sphere::TangentOf(pos.cast_to<normalf>());
		return;
	} while (false);

	float a = d.dot(d);

	do{// …œ∞Î«Ú
		pointf3 center(0, halfH, 0);
		auto oc = o - center;
		float b = d.dot(oc);
		float c = oc.norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray->at(t);
		if (t < ray->tMin || t > ray->tMax || pos[1] <= halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray->at(t);
			if (t < ray->tMin || t > ray->tMax || pos[1] <= halfH)
				break;
		}
		
		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = (pos - center).cast_to<normalf>();
		rst.texcoord = Sphere::TexcoordOf(pos.cast_to<normalf>());
		rst.tangent = Sphere::TangentOf(pos.cast_to<normalf>());
		return;
	} while (false);

	do {// œ¬∞Î«Ú
		pointf3 center(0, -halfH, 0);
		auto oc = o - center;
		float b = d.dot(oc);
		float c = oc.norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray->at(t);
		if (t < ray->tMin || t > ray->tMax || pos[1] >= -halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray->at(t);
			if (t < ray->tMin || t > ray->tMax || pos[1] >= -halfH)
				break;
		}

		rst.isIntersect = true;
		ray->tMax = t;
		rst.n = (pos - center).cast_to<normalf>();
		rst.texcoord = Sphere::TexcoordOf(pos.cast_to<normalf>());
		rst.tangent = Sphere::TangentOf(pos.cast_to<normalf>());
		return;
	} while (false);

	rst.isIntersect = false;
	return;
}
