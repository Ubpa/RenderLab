#include <Engine/VisibilityChecker.h>

#include <Engine/SObj.h>
#include <Engine/Ray.h>

#include <Engine/BVHAccel.h>
#include <Engine/Sphere.h>
#include <Engine/Plane.h>
#include <Engine/Triangle.h>
#include <Engine/TriMesh.h>
#include <Engine/Disk.h>
#include <Engine/Capsule.h>

#include <stack>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

VisibilityChecker::VisibilityChecker() {
	RegMemberFunc<BVHAccel>(&VisibilityChecker::Visit);
	RegMemberFunc<Sphere>(&VisibilityChecker::Visit);
	RegMemberFunc<Plane>(&VisibilityChecker::Visit);
	RegMemberFunc<Triangle>(&VisibilityChecker::Visit);
	RegMemberFunc<Disk>(&VisibilityChecker::Visit);
	RegMemberFunc<Capsule>(&VisibilityChecker::Visit);
}

void VisibilityChecker::Init(const ERay & ray, const float tMax) {
	this->ray = ray;

	this->ray.tMax = tMax;

	rst.isIntersect = false;
}

bool VisibilityChecker::Intersect(const Ubpa::bboxf3 & bbox, const Ubpa::valf3 & invDir) const {
	const auto & origin = ray.o;

	float tMin = ray.tMin;
	float tMax = ray.tMax;

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

void VisibilityChecker::Visit(Ptr<BVHAccel> bvhAccel) {
	const auto visitor = This();

	const auto origin = ray.o;
	const auto dir = ray.d;
	const auto invDir = ray.InvDir();
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
				const auto shape = bvhAccel->GetShape(shapeIdx);

				auto bray = bvhAccel->GetShapeW2LMat(shape) * ray;
				ray.o = bray.o;
				ray.d = bray.d;
				shape->Accept(visitor);

				if (rst.isIntersect)
					return;

				ray.o = origin;
				ray.d = dir;
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
}

void VisibilityChecker::Visit(Ptr<Sphere> sphere) {
	const auto & dir = ray.d;
	const auto & origin = ray.o;

	const Ubpa::vecf3 oc = origin.cast_to<Ubpa::vecf3>();
	const float a = dir.dot(dir);
	const float b = oc.dot(dir);
	const float c = oc.dot(oc) - 1;
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

void VisibilityChecker::Visit(Ptr<Plane> plane) {
	const float t = -ray.o[1] / ray.d[1];
	if (t<ray.tMin || t>ray.tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray(t);
	if (pos[0]<-0.5 || pos[0]>0.5 || pos[2]<-0.5 || pos[2]>0.5) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}

void VisibilityChecker::Visit(Ptr<Triangle> triangle) {
	const auto mesh = triangle->GetMesh();

	const auto & positions = mesh->GetPositions();
	const auto & p1 = positions[triangle->idx[0]];
	const auto & p2 = positions[triangle->idx[1]];
	const auto & p3 = positions[triangle->idx[2]];

	const auto & dir = ray.d;

	const auto e1 = p2 - p1;
	const auto e2 = p3 - p1;

	const auto e1_x_d = e1.cross(dir);
	const float denominator = e1_x_d.dot(e2);

	if (denominator == 0)
		return;

	const float inv_denominator = 1.0f / denominator;

	const auto s = ray.o - p1;

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

	if (t < ray.tMin || t > ray.tMax) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}

void VisibilityChecker::Visit(Ptr<Disk> disk) {
	const float t = -ray.o[1] / ray.d[1];
	if (t<ray.tMin || t>ray.tMax) {
		rst.isIntersect = false;
		return;
	}

	const auto pos = ray(t);
	if (pos.cast_to<Ubpa::vecf3>().norm2() >= 1.f) {
		rst.isIntersect = false;
		return;
	}

	rst.isIntersect = true;
}

void VisibilityChecker::Visit(Ptr<Capsule> capsule) {
	const auto & o = ray.o;
	const auto & d = ray.d;
	const float tMin = ray.tMin;
	const float tMax = ray.tMax;

	float halfH = capsule->height / 2;

	do { // ‘≤÷˘
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
		if (t < tMin || t > tMax) {
			t = (sqrtDiscriminant - b) / a;
			if (t < tMin || t > tMax)
				break;
		}

		auto pos = ray.at(t);
		if (pos[1] <= -halfH || pos[1] >= halfH)
			break;

		rst.isIntersect = true;
		return;
	} while (false);

	float a = d.dot(d);

	do {// …œ∞Î«Ú
		Ubpa::pointf3 center(0, halfH, 0);
		auto oc = o - center;
		float b = d.dot(oc);
		float c = oc.norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray.at(t);
		if (t < tMin || t > tMax || pos[1] <= halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray.at(t);
			if (t < tMin || t > tMax || pos[1] <= halfH)
				break;
		}

		rst.isIntersect = true;
		return;
	} while (false);

	do {// œ¬∞Î«Ú
		Ubpa::pointf3 center(0, -halfH, 0);
		auto oc = o - center;
		float b = d.dot(oc);
		float c = oc.norm2() - 1;

		float discriminant = b * b - a * c;
		if (discriminant <= 0)
			break;

		float sqrtDiscriminant = sqrt(discriminant);
		float t = -(b + sqrtDiscriminant) / a;
		auto pos = ray.at(t);
		if (t < tMin || t > tMax || pos[1] >= -halfH) {
			t = (sqrtDiscriminant - b) / a;
			pos = ray.at(t);
			if (t < tMin || t > tMax || pos[1] >= -halfH)
				break;
		}

		rst.isIntersect = true;
		return;
	} while (false);

	rst.isIntersect = false;
	return;
}
