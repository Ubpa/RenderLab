#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/EleVisitor.h>

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

class BVHAccel::BVHInitVisitor : public EleVisitor {
	ELEVISITOR_SETUP(BVHInitVisitor)
public:
	BVHInitVisitor(BVHAccel * holder)
		: holder(holder) {
		Reg<CmptGeometry>();
		Reg<Sphere>();
		Reg<Plane>();
		Reg<TriMesh>();
	}
private:
	void Visit(CmptGeometry::Ptr geo) {
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

	void Visit(Sphere::Ptr sphere) {
		const auto matrix = holder->GetEleL2WMat(sphere);
		holder->elements.push_back(sphere);
		holder->ele2bbox[sphere] = matrix(sphere->GetBBox());
	}

	void Visit(Plane::Ptr plane) {
		const auto matrix = holder->GetEleL2WMat(plane);
		holder->elements.push_back(plane);
		holder->ele2bbox[plane] = matrix(plane->GetBBox());
	}

	void Visit(TriMesh::Ptr mesh) {
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

const Transform & BVHAccel::GetEleW2LMat(Element::Ptr element) const {
	auto triangle = Triangle::Ptr::Cast(element);
	Primitive::Ptr primitive = triangle ? static_cast<Primitive::Ptr>(triangle->GetMesh()) : Primitive::Ptr::Cast(element);

	const auto target = worldToLocalMatrixes.find(primitive);
	assert(target != worldToLocalMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetEleL2WMat(Element::Ptr element) const {
	auto triangle = Triangle::Ptr::Cast(element);
	Primitive::Ptr primitive = triangle ? static_cast<Primitive::Ptr>(triangle->GetMesh()) : Primitive::Ptr::Cast(element);

	const auto target = localToWorldMatrixes.find(primitive);
	assert(target != localToWorldMatrixes.cend());

	return target->second;
}

const Transform & BVHAccel::GetSObjL2WMat(Ptr<SObj> sobj) const {
	const auto target = sobjL2W.find(sobj);
	assert(target != sobjL2W.cend());

	return target->second;
}

const Ptr<SObj> BVHAccel::GetSObj(Element::Ptr element) const{
	auto triangle = Triangle::Ptr::Cast(element);
	Primitive::Ptr primitive = triangle ? static_cast<Primitive::Ptr>(triangle->GetMesh()) : Primitive::Ptr::Cast(element);

	const auto target = primitive2sobj.find(primitive);
	assert(target != primitive2sobj.cend());

	return target->second;
}

const BBoxf & BVHAccel::GetBBox(Element::Ptr element) const {
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
	auto initVisitor = ToPtr(new BVHInitVisitor(this));
	for (auto geo : geos)
		geo->Accept(initVisitor);
	bvhRoot = ToPtr(new BVHNode<Element, BVHAccel>(this, elements, 0, elements.size()));
}
