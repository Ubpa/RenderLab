#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/BVHNode.h>

#include <CppUtil/Basic/Visitor.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

BVHAccel::BVHAccel()
	: bvhRoot(nullptr) { }

// ------------ BVHInitVisitor ------------

class BVHAccel::BVHInitVisitor : public Visitor {
public:
	BVHInitVisitor(BVHAccel * holder)
		: holder(holder) {
		RegMemberFunc<CmptGeometry>(&BVHAccel::BVHInitVisitor::Visit);
		RegMemberFunc<Sphere>(&BVHAccel::BVHInitVisitor::Visit);
		RegMemberFunc<Plane>(&BVHAccel::BVHInitVisitor::Visit);
		RegMemberFunc<TriMesh>(&BVHAccel::BVHInitVisitor::Visit);
	}

public:
	static const Ptr<BVHInitVisitor> New(BVHAccel * holder) {
		return Basic::New<BVHInitVisitor>(holder);
	}

protected:
	virtual ~BVHInitVisitor() = default;

public:
	void Visit(Ptr<CmptGeometry> geo) {
		auto primitive = geo->primitive;
		if (!primitive)
			return;

		const auto w2l = geo->GetSObj()->GetWorldToLocalMatrix();
		const auto l2w = w2l.Inverse();
		holder->worldToLocalMatrixes[primitive] = w2l;
		holder->localToWorldMatrixes[primitive] = l2w;
		holder->sobjL2W[geo->GetSObj()] = l2w;

		holder->primitive2sobj[geo->primitive] = geo->GetSObj();
		geo->primitive->Accept(This());
	}

	void Visit(Ptr<Sphere> sphere) {
		const auto matrix = holder->GetShapeL2WMat(sphere);
		holder->shapes.push_back(sphere);
		holder->shape2wbbox[sphere] = matrix(sphere->GetBBox());
	}

	void Visit(Ptr<Plane> plane) {
		const auto matrix = holder->GetShapeL2WMat(plane);
		holder->shapes.push_back(plane);
		holder->shape2wbbox[plane] = matrix(plane->GetBBox());
	}

	void Visit(Ptr<TriMesh> mesh) {
		const auto matrix = holder->GetShapeL2WMat(mesh);
		auto triangles = mesh->GetTriangles();
		for (auto triangle : triangles) {
			holder->shapes.push_back(triangle);
			holder->shape2wbbox[triangle] = matrix(triangle->GetBBox());
		}
	}

private:
	BVHAccel * holder;
};

const Transform & BVHAccel::GetShapeW2LMat(Ptr<Shape> shape) const {
	const auto target = worldToLocalMatrixes.find(shape->GetPrimitive());
	assert(target != worldToLocalMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetShapeL2WMat(Ptr<Shape> shape) const {
	const auto target = localToWorldMatrixes.find(shape->GetPrimitive());
	assert(target != localToWorldMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetSObjL2WMat(Ptr<SObj> sobj) const {
	const auto target = sobjL2W.find(sobj);
	assert(target != sobjL2W.cend());

	return target->second;
}

const Ptr<SObj> BVHAccel::GetSObj(Ptr<Shape> shape) const{
	const auto target = primitive2sobj.find(shape->GetPrimitive());
	assert(target != primitive2sobj.cend());

	return target->second;
}

const BBoxf & BVHAccel::GetWorldBBox(Ptr<Shape> shape) const {
	const auto target = shape2wbbox.find(shape);
	assert(target != shape2wbbox.cend());

	return target->second;
}

void BVHAccel::Clear() {
	worldToLocalMatrixes.clear();
	localToWorldMatrixes.clear();
	sobjL2W.clear();
	primitive2sobj.clear();
	shapes.clear();
	shape2wbbox.clear();

	bvhRoot = nullptr;
}

void BVHAccel::Init(Ptr<SObj> root) {
	Clear();

	auto geos = root->GetComponentsInChildren<CmptGeometry>();
	auto initVisitor = BVHInitVisitor::New(this);
	for (auto geo : geos)
		geo->Accept(initVisitor);
	bvhRoot = BVHNode::New(this, shapes, 0, shapes.size());
}
