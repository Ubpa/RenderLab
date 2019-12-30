#include <CppUtil/Engine/MeshEdit/LoopSubdivision.h>

#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Basic/Math.h>
#include <thread>

#include <omp.h>

#ifdef NDEBUG
#define THREAD_NUM omp_get_num_procs() - 1
#else
#define THREAD_NUM 1
//#define THREAD_NUM omp_get_num_procs() - 1
#endif //  NDEBUG

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
	
	triMesh->Init(indice, positions);

	return true;
}

void LoopSubdivision::HEMeshSubdivision() {
	// 1. update vertex pos
	{
		auto & vertices = heMesh->Vertices();
		auto work = [&vertices](size_t id) {
			for (size_t i = id; i < vertices.size(); i += THREAD_NUM) {
				auto v = vertices[i];
				auto adjVs = v->AdjVertices();
				size_t n = adjVs.size();
				float u = n == 3 ? 3.f / 16.f : 3.f / (8.f*n);
				Vec3 sumPos;
				for (auto adjV : adjVs)
					sumPos += adjV->pos;
				v->newPos = (1.f - n * u)*v->pos + u * sumPos;
				v->isNew = false;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}

	// 2. compute pos of new vertice on edges
	{
		auto & edges = heMesh->Edges();
		auto work = [&edges](size_t id) {
			for (size_t i = id; i < edges.size(); i += THREAD_NUM) {
				auto e = edges[i];
				auto pos0 = e->HalfEdge()->Origin()->pos;
				auto pos1 = e->HalfEdge()->Pair()->Origin()->pos;
				auto pos2 = e->HalfEdge()->Next()->End()->pos;
				auto pos3 = e->HalfEdge()->Pair()->Next()->End()->pos;

				e->newPos = (3.f*(pos0 + pos1) + pos2 + pos3) / 8.f;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}
	
	// 3. spilt edges
	auto & edges = heMesh->Edges();
	size_t nE = edges.size();
	vector<Ptr<E>> newEdges;
	newEdges.reserve(2 * nE);
	for (int i = 0; i < nE; i++) { // when i go up, edges change, but still can iterate 
		auto e = edges[i];

		auto v = heMesh->SpiltEdge(e); // e remains in container
		v->isNew = true;
		v->newPos = e->newPos;

		auto he = e->HalfEdge();
		newEdges.push_back(he->Next()->Next()->Edge());
		newEdges.push_back(he->Pair()->Next()->Edge());
	}

	// 4. flip new edge with old and new vertex
	for (auto e : newEdges) {
		if (e->HalfEdge()->Origin()->isNew + e->HalfEdge()->Pair()->Origin()->isNew != 1 )
			continue;

		heMesh->RotateEdge(e);
	}

	// 5. update vertex pos
	{
		for (auto v : heMesh->Vertices())
			v->pos = v->newPos;

		auto & vertices = heMesh->Vertices();
		auto work = [&vertices](size_t id) {
			for (size_t i = id; i < vertices.size(); i += THREAD_NUM) {
				auto v = vertices[i];
				v->pos = v->newPos;
			}
		};

		vector<thread> workers;
		for (int i = 0; i < THREAD_NUM; i++)
			workers.push_back(thread(work, i));

		// wait workers
		for (auto & worker : workers)
			worker.join();
	}
}
