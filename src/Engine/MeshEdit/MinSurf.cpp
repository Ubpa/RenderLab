#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/TriMesh.h>

#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

MinSurf::MinSurf(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>())
{
	Init(triMesh);
}

void MinSurf::Clear() {
	heMesh->Clear();
	triMesh = nullptr;
}

bool MinSurf::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	heMesh->Init(triangles);

	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	if (!heMesh->IsTriMesh() || !heMesh->HaveBoundary()) {
		printf("ERROR::MinSurf::Init:\n"
			"\t""trimesh is not a triangle mesh or hasn't a boundaries\n");
		heMesh->Clear();
		return false;
	}

	this->triMesh = triMesh;
	return true;
}

bool MinSurf::Run() {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::MinSurf::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
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
	vector<pointf3> positions;
	vector<unsigned> indice;
	positions.reserve(nV);
	indice.reserve(3 * nF);
	for (auto v : heMesh->Vertices())
		positions.push_back(v->pos.cast_to<pointf3>());
	for (auto f : heMesh->Polygons()) {
		for (auto v : f->BoundaryVertice())
			indice.push_back(static_cast<unsigned>(heMesh->Index(v)));
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
			Ex(vIdx) = v->pos[0];
			Ey(vIdx) = v->pos[1];
			Ez(vIdx) = v->pos[2];
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
			v->pos = vecf3(newX(id), newY(id), newZ(id));
		}
	}
}