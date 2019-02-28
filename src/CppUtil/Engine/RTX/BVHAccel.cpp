#include <CppUtil/Engine/BVHAccel.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Basic/EleVisitor.h>

#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

BVHAccel::BVHAccel()
	: bvhRoot(nullptr) {

}


// ------------ BVHInitVisitor ------------

class BVHAccel::BVHInitVisitor : public EleVisitor {
	ELEVISITOR_SETUP(BVHInitVisitor)
public:
	BVHInitVisitor(BVHAccel * holder)
		: holder(holder) {
		Reg<Geometry>();
		Reg<Sphere>();
		Reg<Plane>();
		Reg<TriMesh>();
	}
private:
	void Visit(Geometry::Ptr geo) {
		auto primitive = geo->GetPrimitive();
		if (!primitive)
			return;

		auto w2l = geo->GetSObj()->GetWorldToLocalMatrix();
		auto l2w = inverse(w2l);
		holder->worldToLocalMatrixes[primitive] = w2l;
		holder->localToWorldMatrixes[primitive] = l2w;
		holder->norm_localToWorld[geo->GetSObj()] = transpose(inverse(mat3(l2w)));

		holder->primitive2sobj[geo->GetPrimitive()] = geo->GetSObj();
		geo->GetPrimitive()->Accept(This());
	}

	void Visit(Sphere::Ptr sphere) {
		auto matrix = holder->localToWorldMatrixes[sphere];
		holder->elements.push_back(sphere);
		holder->ele2bbox[sphere] = sphere->GetBBox().Transform(matrix);
	}

	void Visit(Plane::Ptr plane) {
		auto matrix = holder->localToWorldMatrixes[plane];
		holder->elements.push_back(plane);
		holder->ele2bbox[plane] = plane->GetBBox().Transform(matrix);
	}

	void Visit(TriMesh::Ptr mesh) {
		auto matrix = holder->localToWorldMatrixes[mesh];
		auto triangles = mesh->GetTriangles();
		for (auto triangle : triangles) {
			holder->elements.push_back(triangle);
			holder->ele2bbox[triangle] = triangle->GetBBox().Transform(matrix);
		}
	}

private:
	BVHAccel * holder;
};

// ------------ GetPrimitiveVisitor ------------

class BVHAccel::GetPrimitiveVisitor : public EleVisitor {
	ELEVISITOR_SETUP(GetPrimitiveVisitor)
public:
	GetPrimitiveVisitor() {
		Reg<Triangle>();
		Reg<Sphere>();
		Reg<Plane>();
		Reg<TriMesh>();
	}
private:
	void Visit(Triangle::Ptr obj) { primitive = obj->GetMesh(); }
	void Visit(Sphere::Ptr obj) { primitive = obj; }
	void Visit(Plane::Ptr obj) { primitive = obj; }
	void Visit(TriMesh::Ptr obj) { primitive = obj; }
public:
	Primitive::Ptr primitive;
};

const mat4 & BVHAccel::GetEleW2LMat(Element::Ptr element) {
	auto triangle = Triangle::Ptr::Cast(element);
	if(triangle)
		return worldToLocalMatrixes[triangle->GetMesh()];
	else
		return worldToLocalMatrixes[Primitive::Ptr::Cast(element)];
}

const mat4 & BVHAccel::GetEleL2WMat(Element::Ptr element) {
	auto triangle = Triangle::Ptr::Cast(element);
	if (triangle)
		return localToWorldMatrixes[triangle->GetMesh()];
	else
		return localToWorldMatrixes[Primitive::Ptr::Cast(element)];
}

const SObj::Ptr BVHAccel::GetSObj(Element::Ptr element) {
	auto triangle = Triangle::Ptr::Cast(element);
	if (triangle)
		return primitive2sobj[triangle->GetMesh()];
	else
		return primitive2sobj[Primitive::Ptr::Cast(element)];
}

void BVHAccel::Init(SObj::Ptr root) {
	norm_localToWorld.clear();
	worldToLocalMatrixes.clear();
	localToWorldMatrixes.clear();
	primitive2sobj.clear();
	elements.clear();
	ele2bbox.clear();

	auto geos = root->GetComponentsInChildren<Geometry>();
	auto initVisitor = ToPtr(new BVHInitVisitor(this));
	for (auto geo : geos)
		geo->Accept(initVisitor);
	bvhRoot = ToPtr(new BVHNode<Element, BVHAccel>(this, elements, 0, elements.size()));
}
