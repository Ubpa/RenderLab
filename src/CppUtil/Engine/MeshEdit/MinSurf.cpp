#include <CppUtil/Engine/MeshEdit/MinSurf.h>

#include <CppUtil/Engine/TriMesh.h>

#include <Eigen/Sparse>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;
using namespace Eigen;

void MinSurf::Clear() {
	heMesh = nullptr;
	triMesh = nullptr;
}

bool MinSurf::Init(Basic::Ptr<TriMesh> triMesh) {
	if (triMesh == this->triMesh)
		return true;

	Clear();

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	size_t nF = triMesh->GetTriangles().size();
	heMesh = HEMesh<V>::New();
	heMesh->Reserve(nV);
	for (int i = 0; i < nV; i++) {
		auto v = heMesh->AddVertex();
		v->pos = triMesh->GetPositions()[i];
	}

	auto & vertice = heMesh->Vertices();
	for (auto f : triMesh->GetTriangles()) {
		auto v0 = vertice[f->idx[0]];
		auto v1 = vertice[f->idx[1]];
		auto v2 = vertice[f->idx[2]];
		auto he01 = V::FindHalfEdge(v0, v1);
		auto he12 = V::FindHalfEdge(v1, v2);
		auto he20 = V::FindHalfEdge(v2, v0);
		if (!he01)
			he01 = heMesh->AddEdge(v0, v1)->HalfEdge();
		if (!he12)
			he12 = heMesh->AddEdge(v1, v2)->HalfEdge();
		if (!he20)
			he20 = heMesh->AddEdge(v2, v0)->HalfEdge();
		heMesh->AddPolygon({ he01,he12,he20 });
	}

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		this->heMesh = nullptr;
		return false;
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (!heMesh || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""!heMesh || !triMesh\n");
		return false;
	}

	Minimize();

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""!heMesh->IsTriMesh() || !heMesh->HaveBoundary(), algorithm error\n");
		return false;
	}

	size_t nV = heMesh->NumVertices();
	size_t nF = heMesh->NumPolygons();
	vector<Point3> positions;
	vector<uint> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos);
	for (auto f : heMesh->Polygons()) {
		for (auto v : f->BoundaryVertice())
			indice.push_back(static_cast<uint>(heMesh->Index(v)));
	}

	triMesh->Init(indice, positions);

	return true;
}

void MinSurf::Minimize() {
	auto vertices = heMesh->Vertices();
	size_t n = vertices.size();
	SparseMatrix<float> coefficient;
	coefficient.resize(n, n);
	VectorXf Ex;
	VectorXf Ey;
	VectorXf Ez;
	Ex.resize(n);
	Ey.resize(n);
	Ez.resize(n);
	Ex.setZero();
	Ey.setZero();
	Ez.setZero();
	std::vector<Triplet<float>> triplets;
	for (auto v : heMesh->Vertices()) {
		auto vIdx = static_cast<int>(heMesh->Index(v));
		if (v->IsBoundary()) {
			Ex(vIdx) = v->pos.x;
			Ey(vIdx) = v->pos.y;
			Ez(vIdx) = v->pos.z;
			triplets.push_back({ vIdx, vIdx, 1.f });		// 初始化非零元素
		}
		else {
			for (auto u : v->AdjVertices()) {
				auto id = static_cast<int>(heMesh->Index(u));
				triplets.push_back({ vIdx, id, 1.f });
			}
			triplets.push_back({ vIdx, vIdx, -static_cast<float>(v->Degree()) });
		}
	}
	coefficient.setFromTriplets(triplets.begin(), triplets.end());

	BiCGSTAB<SparseMatrix<float>> solver(coefficient);

	//SparseQR< SparseMatrix<float>,AMDOrdering<int> > qr;
	//qr.compute(coefficient);//速度太慢

	VectorXf newX(n);
	VectorXf newY(n);
	VectorXf newZ(n);

	newX = solver.solve(Ex);
	newY = solver.solve(Ey);
	newZ = solver.solve(Ez);

	for (auto v : heMesh->Vertices()) {
		if (!v->IsBoundary()) {
			auto id = heMesh->Index(v);
			v->pos = Vec3(newX(id), newY(id), newZ(id));
		}
	}
}