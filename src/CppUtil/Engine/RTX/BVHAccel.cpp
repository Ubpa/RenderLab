#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/Visitor.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

BVHAccel::BVHAccel()
	: bvhRoot(nullptr) { }

// ------------ BVHInitVisitor ------------

class BVHAccel::BVHInitVisitor : public Visitor<BVHAccel::BVHInitVisitor> {
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
		const auto matrix = holder->GetEleL2WMat(sphere);
		holder->elements.push_back(sphere);
		holder->ele2bbox[sphere] = matrix(sphere->GetBBox());
	}

	void Visit(Ptr<Plane> plane) {
		const auto matrix = holder->GetEleL2WMat(plane);
		holder->elements.push_back(plane);
		holder->ele2bbox[plane] = matrix(plane->GetBBox());
	}

	void Visit(Ptr<TriMesh> mesh) {
		const auto matrix = holder->GetEleL2WMat(mesh);
		auto triangles = mesh->GetTriangles();
		for (auto triangle : triangles) {
			holder->elements.push_back(triangle);
			holder->ele2bbox[triangle] = matrix(triangle->GetBBox());
		}
	}

private:
	BVHAccel * holder;
};

const Transform & BVHAccel::GetEleW2LMat(Ptr<ElementBase> element) const {
	Ptr<PrimitiveBase> primitive = nullptr;
	auto triangle = Triangle::PtrCast(element);
	if (triangle)
		primitive = triangle->GetMesh();
	else
		primitive = dynamic_pointer_cast<PrimitiveBase>(element);

	const auto target = worldToLocalMatrixes.find(primitive);
	assert(target != worldToLocalMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetEleL2WMat(Ptr<ElementBase> element) const {
	Ptr<PrimitiveBase> primitive = nullptr;
	auto triangle = Triangle::PtrCast(element);
	if (triangle)
		primitive = triangle->GetMesh();
	else
		primitive = dynamic_pointer_cast<PrimitiveBase>(element);

	const auto target = localToWorldMatrixes.find(primitive);
	assert(target != localToWorldMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetSObjL2WMat(Ptr<SObj> sobj) const {
	const auto target = sobjL2W.find(sobj);
	assert(target != sobjL2W.cend());

	return target->second;
}

const Ptr<SObj> BVHAccel::GetSObj(Ptr<ElementBase> element) const{
	Ptr<PrimitiveBase> primitive = nullptr;
	auto triangle = Triangle::PtrCast(element);
	if (triangle)
		primitive = triangle->GetMesh();
	else
		primitive = dynamic_pointer_cast<PrimitiveBase>(element);

	const auto target = primitive2sobj.find(primitive);
	assert(target != primitive2sobj.cend());

	return target->second;
}

const BBoxf & BVHAccel::GetBBox(Ptr<ElementBase> element) const {
	const auto target = ele2bbox.find(element);
	assert(target != ele2bbox.cend());

	return target->second;
}

void BVHAccel::Clear() {
	worldToLocalMatrixes.clear();
	localToWorldMatrixes.clear();
	sobjL2W.clear();
	primitive2sobj.clear();
	elements.clear();
	ele2bbox.clear();

	bvhRoot = nullptr;
}

void BVHAccel::Init(Ptr<SObj> root) {
	Clear();

	auto geos = root->GetComponentsInChildren<CmptGeometry>();
	auto initVisitor = BVHInitVisitor::New(this);
	for (auto geo : geos)
		geo->Accept(initVisitor);
	bvhRoot = BVHNode<ElementBase, BVHAccel>::New(this, elements, 0, elements.size());
}
