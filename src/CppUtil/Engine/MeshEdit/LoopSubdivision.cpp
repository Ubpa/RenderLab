#include <CppUtil/Engine/MeshEdit/LoopSubdivision.h>

#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

void LoopSubdivision::Clear() {
	triMesh = nullptr;
	heMesh = nullptr;
}

bool LoopSubdivision::Init(Basic::Ptr<TriMesh> triMesh) {
	if (triMesh == this->triMesh)
		return true;

	Clear();

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::LoopSubdivision::Init:\n"
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

	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::LoopSubdivision::Init:\n"
			"\t""trimesh is not a triangle mesh or has boundaries\n");
		this->heMesh = nullptr;
		return false;
	}

	this->triMesh = triMesh;
	return true;
}

bool LoopSubdivision::Run(size_t n){
	if (!heMesh || !triMesh) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""!heMesh || !triMesh\n");
		return false;
	}

	for (size_t i = 0; i < n; i++)
		HEMeshSubdivision();

	if (!heMesh->IsTriMesh() || heMesh->HaveBoundary()) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""!heMesh->IsTriMesh() || heMesh->HaveBoundary(), algorithm error\n");
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
	
	//auto normals = triMesh->GetNormals();
	//auto origIdx = triMesh->GetIndice();
	//auto origPos = triMesh->GetPositions();
	triMesh->Init(indice, positions);

	return true;
}

void LoopSubdivision::HEMeshSubdivision() {
	// update vertex pos
	for (auto v : heMesh->Vertices()) {
		auto adjVs = v->AdjVertices();
		size_t n = adjVs.size();
		float u = n == 3 ? 3.f / 16.f : 3.f / (8.f*n);
		Vec3 sumPos;
		for (auto adjV : adjVs)
			sumPos += adjV->pos;
		v->newPos = (1.f-n*u)*v->pos + u * sumPos;
		v->isNew = false;
	}

	// compute pos of new vertice on edges
	for (auto e : heMesh->Edges()) {
		auto pos0 = e->HalfEdge()->Origin()->pos;
		auto pos1 = e->HalfEdge()->Pair()->Origin()->pos;
		auto pos2 = e->HalfEdge()->Next()->End()->pos;
		auto pos3 = e->HalfEdge()->Pair()->Next()->End()->pos;

		e->newPos = (3.f*(pos0 + pos1) + pos2 + pos3) / 8.f;
		e->isNew = false;
	}

	// spilt edges
	auto & edges = heMesh->Edges();
	size_t nE = edges.size();
	for (int i = 0; i < nE; i++) { // when i go up, edges change, but still can iterate 
		auto e = edges[i];

		auto v = heMesh->SpiltEdge(e); // e remains in container
		v->isNew = true;
		v->newPos = e->newPos;

		auto he = e->HalfEdge();
		he->Next()->Next()->Edge()->isNew = true;
		he->Pair()->Next()->Edge()->isNew = true;
	}

	// flip new edge with old and new vertex
	for (auto e : heMesh->Edges()) {
		if (!e->isNew || e->HalfEdge()->Origin()->isNew + e->HalfEdge()->Pair()->Origin()->isNew != 1 )
			continue;

		heMesh->RotateEdge(e);
	}

	for (auto v : heMesh->Vertices())
		v->pos = v->newPos;
}
