#include <Engine/Viewer/BVHAccel.h>

#include "BVHNode.h"

#include <Engine/Primitive/Sphere.h>
#include <Engine/Primitive/Plane.h>
#include <Engine/Primitive/TriMesh.h>
#include <Engine/Primitive/Disk.h>
#include <Engine/Primitive/Capsule.h>

#include <Engine/Scene/CmptGeometry.h>

#include <Engine/Scene/SObj.h>

#include <Basic/Timer.h>

#include <UDP/Visitor.h>

using namespace std;
using namespace Ubpa;

// ------------ BVHInitVisitor ------------

class BVHAccel::BVHInitVisitor final : public SharedPtrVisitor<BVHAccel::BVHInitVisitor, Primitive>, public HeapObj{
public:
	BVHInitVisitor(BVHAccel * holder) : holder(holder) {
		Regist<Sphere, Plane, TriMesh, Disk, Capsule>();
	}

public:
	unordered_map<Ptr<Shape>, bboxf3> shape2wbbox;

public:
	static const Ptr<BVHInitVisitor> New(BVHAccel * holder) {
		return Ubpa::New<BVHInitVisitor>(holder);
	}

protected:
	virtual ~BVHInitVisitor() = default;

public:
	using SharedPtrVisitor<BVHAccel::BVHInitVisitor, Primitive>::Visit;
	void Visit(Ptr<CmptGeometry> geo) {
		auto primitive = geo->primitive;
		if (!primitive)
			return;

		const auto w2l = geo->GetSObj()->GetLocalToWorldMatrix().inverse();
		holder->worldToLocalMatrixes[primitive] = w2l;

		holder->primitive2sobj[geo->primitive] = geo->GetSObj();
		Visit(geo->primitive);
	}

protected:
	void ImplVisit(Ptr<Sphere> sphere) {
		const auto l2w = holder->GetShapeW2LMat(sphere).inverse();
		holder->shapes.push_back(sphere);
		shape2wbbox[sphere] = l2w * sphere->GetBBox();
	}

	void ImplVisit(Ptr<Plane> plane) {
		const auto l2w = holder->GetShapeW2LMat(plane).inverse();
		holder->shapes.push_back(plane);
		shape2wbbox[plane] = l2w * plane->GetBBox();
	}

	void ImplVisit(Ptr<TriMesh> mesh) {
		const auto l2w = holder->GetShapeW2LMat(mesh).inverse();
		for (auto triangle : mesh->GetTriangles()) {
			holder->shapes.push_back(triangle);
			shape2wbbox[triangle] = l2w * triangle->GetBBox();
		}
	}

	void ImplVisit(Ptr<Disk> disk) {
		const auto l2w = holder->GetShapeW2LMat(disk).inverse();
		holder->shapes.push_back(disk);
		shape2wbbox[disk] = l2w * disk->GetBBox();
	}

	void ImplVisit(Ptr<Capsule> capsule) {
		const auto l2w = holder->GetShapeW2LMat(capsule).inverse();
		holder->shapes.push_back(capsule);
		shape2wbbox[capsule] = l2w * capsule->GetBBox();
	}

private:
	BVHAccel * holder;
};

const transformf & BVHAccel::GetShapeW2LMat(Ptr<Shape> shape) const {
	const auto target = worldToLocalMatrixes.find(shape->GetPrimitive());
	assert(target != worldToLocalMatrixes.cend());

	return target->second;
}

const Ptr<SObj> BVHAccel::GetSObj(Ptr<Shape> shape) const{
	const auto target = primitive2sobj.find(shape->GetPrimitive());
	assert(target != primitive2sobj.cend());

	return target->second;
}

void BVHAccel::Clear() {
	worldToLocalMatrixes.clear();
	primitive2sobj.clear();
	shapes.clear();
	linearBVHNodes.clear();
}

void BVHAccel::Init(Ptr<SObj> root) {
	Clear();

	auto geos = root->GetComponentsInChildren<CmptGeometry>();
	auto initVisitor = BVHInitVisitor::New(this);
	for (auto geo : geos)
		initVisitor->Visit(geo);

	printf("Building BVH...\n");
	Timer timer;
	timer.Start();
	const auto bvhRoot = BVHNode::New(initVisitor->shape2wbbox, shapes, 0, shapes.size());
	LinearizeBVH(bvhRoot);
	timer.Stop();
	printf("BVH build done, cost %f s\n", timer.GetWholeTime());
}

void BVHAccel::LinearizeBVH(Ptr<BVHNode> bvhNode) {
	linearBVHNodes.push_back(LinearBVHNode());
	const auto curNodeIdx = linearBVHNodes.size() - 1;

	if (!bvhNode->IsLeaf()) {
		LinearizeBVH(bvhNode->GetL());
		linearBVHNodes[curNodeIdx].InitBranch(bvhNode->GetBBox(), static_cast<int>(linearBVHNodes.size()), bvhNode->GetAxis());
		LinearizeBVH(bvhNode->GetR());
	}
	else
		linearBVHNodes[curNodeIdx].InitLeaf(bvhNode->GetBBox(), static_cast<int>(bvhNode->GetShapeOffset()), static_cast<int>(bvhNode->GetShapesNum()));
}
