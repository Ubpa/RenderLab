#include <Engine/MeshEdit/LoopSubdivision.h>

#include <Engine/TriMesh.h>
#include <Basic/Math.h>
#include <Basic/Parallel.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;
using namespace Ubpa;

LoopSubdivision::LoopSubdivision(Basic::Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>()) {
	Init(triMesh);
}

void LoopSubdivision::Clear() {
	triMesh = nullptr;
	heMesh->Clear();
}

bool LoopSubdivision::Init(Basic::Ptr<TriMesh> triMesh) {
	Clear();
	if (triMesh == nullptr)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	auto initSuccess = heMesh->Init(triangles);
	if (!initSuccess) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""HEMesh init fail\n");
		return false;
	}

	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i];
	}

	if (!heMesh->IsTriMesh()) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""!heMesh->IsTriMesh()\n");
		heMesh->Clear();
		return false;
	}

	this->triMesh = triMesh;
	return true;
}

bool LoopSubdivision::Run(size_t n) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	heMesh->Reserve(static_cast<size_t>(pow(4, n)) * heMesh->NumVertices());
	for (size_t i = 0; i < n; i++)
		Kernel();

	if (!heMesh->IsTriMesh()) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""!heMesh->IsTriMesh(), algorithm error\n");
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
	for (auto triangle : heMesh->Export()) {
		for (auto idx : triangle)
			indice.push_back(static_cast<uint>(idx));
	}

	triMesh->Init(indice, positions);

	return true;
}

void LoopSubdivision::Kernel() {
	// 1. update vertex pos
	printf("1. update vertex pos\n");
	auto step1 = [](V* v) {
		auto adjVs = v->AdjVertices();
		if (v->IsBoundary()) {
			Vec3 sumPos;
			for (auto adjV : adjVs) {
				if (adjV->IsBoundary())
					sumPos += adjV->pos;
			}
			v->newPos = 3.f / 4.f * v->pos + 1.f / 8.f * sumPos;
		}
		else {
			size_t n = adjVs.size();
			float u = n == 3 ? 3.f / 16.f : 3.f / (8.f * n);
			Vec3 sumPos;
			for (auto adjV : adjVs)
				sumPos += adjV->pos;
			v->newPos = (1.f - n * u) * v->pos + u * sumPos;
		}
		v->isNew = false;
	};
	Parallel::Instance().Run(step1, heMesh->Vertices());

	// 2. compute pos of new vertice on edges
	printf("2. compute pos of new vertice on edges\n");
	auto setp2 = [](E* e) {
		auto pos0 = e->HalfEdge()->Origin()->pos;
		auto pos1 = e->HalfEdge()->Pair()->Origin()->pos;
		if (!e->IsBoundary()) {
			auto pos2 = e->HalfEdge()->Next()->End()->pos;
			auto pos3 = e->HalfEdge()->Pair()->Next()->End()->pos;

			e->newPos = (3.f * (pos0 + pos1) + pos2 + pos3) / 8.f;
		}
		else
			e->newPos = (pos0 + pos1) / 2.f;
	};
	Parallel::Instance().Run(setp2, heMesh->Edges());

	// 3. spilt edges
	printf("3. spilt edges\n");
	auto edges = heMesh->Edges(); // must copy
	vector<E*> newEdges;
	newEdges.reserve(2 * edges.size());
	for (auto e : edges) {
		auto v0 = e->HalfEdge()->Origin();
		auto v1 = e->HalfEdge()->End();

		auto newPos = e->newPos;
		auto v = heMesh->SpiltEdge(e); // e is deleted in HEMesh
		v->isNew = true;
		v->newPos = newPos;

		for (auto he : v->OutHEs()) {
			if (he->End() != v0 && he->End() != v1)
				newEdges.push_back(he->Edge());
		}
	}

	// 4. flip new edge with old and new vertex
	printf("4. flip new edge with old and new vertex\n");
	for (auto e : newEdges) {
		if (e->HalfEdge()->Origin()->isNew + e->HalfEdge()->Pair()->Origin()->isNew != 1)
			continue;

		heMesh->FlipEdge(e);
	}

	// 5. update vertex pos
	printf("5. update vertex pos\n");
	auto step5 = [](V* v) { v->pos = v->newPos; };
	Parallel::Instance().Run(step5, heMesh->Vertices());
}
