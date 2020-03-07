#include <Engine/MeshEdit/IsotropicRemeshing.h>

#include <Engine/Primitive/TriMesh.h>
#include <Basic/Math.h>
#include <Basic/Parallel.h>
#include <Basic/Geometry.h>

#include <unordered_set>
#include <set>
#include <array>
#include <tuple>
#include <mutex>

using namespace Ubpa;

using namespace std;
using namespace Ubpa;

IsotropicRemeshing::IsotropicRemeshing(Ptr<TriMesh> triMesh)
	: heMesh(make_shared<HEMesh<V>>()) {
	Init(triMesh);
}

void IsotropicRemeshing::Clear() {
	triMesh = nullptr;
	heMesh->Clear();
}

bool IsotropicRemeshing::Init(Ptr<TriMesh> triMesh) {
	Clear();

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""trimesh is invalid\n");
		return false;
	}

	size_t nV = triMesh->GetPositions().size();
	vector<vector<size_t>> triangles;
	triangles.reserve(triMesh->GetTriangles().size());
	for (auto triangle : triMesh->GetTriangles())
		triangles.push_back({ triangle->idx[0], triangle->idx[1], triangle->idx[2] });
	heMesh->Reserve(nV);
	if (!heMesh->Init(triangles)) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""HEMesh init fail\n");
		return false;
	}

	if (!heMesh->IsTriMesh()) {
		printf("ERROR::IsotropicRemeshing::Init:\n"
			"\t""trimesh is not a triangle mesh\n");
		heMesh->Clear();
		return false;
	}

	for (int i = 0; i < nV; i++) {
		auto v = heMesh->Vertices().at(i);
		v->pos = triMesh->GetPositions()[i].cast_to<vecf3>();
	}

	this->triMesh = triMesh;
	return true;
}

bool IsotropicRemeshing::Run(size_t n) {
	if (heMesh->IsEmpty() || !triMesh) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""heMesh->IsEmpty() || !triMesh\n");
		return false;
	}

	bool success = Kernel(n);
	if (!success) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""run Kernel fail\n");
		return false;
	}

	if (!heMesh->IsTriMesh()) {
		printf("ERROR::IsotropicRemeshing::Run\n"
			"\t""!heMesh->IsTriMesh(), algorithm error\n");
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
	for (auto triangle : heMesh->Export()) {
		for (auto idx : triangle)
			indice.push_back(static_cast<unsigned>(idx));
	}

	triMesh->Init(indice, positions);

	return true;
}

bool IsotropicRemeshing::Kernel(size_t n) {
	if (heMesh->NumPolygons() == 2)
		return true; // dihedron

	// 1. mean of edges length
	printf("1. mean of edges length\n");
	auto step1 = [](E* e) {return e->Length(); };
	float L = Parallel::Instance().RunSum(step1, heMesh->Edges()) / heMesh->NumEdges();
	float minL = 0.8f * L;
	float maxL = 4.f / 3.f * L;

	for (size_t i = 0; i < n; i++) {
		// 2. spilt edges with length > maxL
		{
			printf("2. spilt edges with length > maxL\n");
			unordered_set<E*> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
			while (dEs.size() > 0) {
				auto iter = dEs.begin();
				auto e = *iter;
				dEs.erase(iter);

				if (e->Length() > maxL) {
					auto v = heMesh->SpiltEdge(e, e->Centroid());

					for (auto adjE : v->AdjEdges())
						dEs.insert(adjE);
				}
			}
		}

		// 3. collapse edges with length < minL
		{
			printf("3. collapse edges with length < minL\n");
			unordered_set<E*> dEs(heMesh->Edges().begin(), heMesh->Edges().end()); // dynamic edges
			while (dEs.size() > 0) {
				auto iter = dEs.begin();
				auto e = *iter;
				dEs.erase(iter);

				if (!e->IsCanCollapse(minL, maxL))
					continue;

				auto eAdjEs = e->AdjEdges();
				if (eAdjEs.size() <= 2)
					continue; // dihedron

				auto v = heMesh->CollapseEdge(e, e->Centroid());
				if (v != nullptr) {
					for (auto eAdjE : eAdjEs)
						dEs.erase(eAdjE);
					for (auto adjE : v->AdjEdges())
						dEs.insert(adjE);
				}
			}
		}

		// 4. filp edges which can balance degree
		printf("4. filp edges which can balance degree\n");
		vector<mutex> vertexMutexes(heMesh->NumVertices());
		auto step4 = [&](E* e) {
			if (e->IsBoundary())
				return;
			auto he01 = e->HalfEdge();
			auto he10 = he01->Pair();

			// lock 4 vertices
			set<size_t> sortedIndices; // avoid deadlock
			array<V*, 4> vertices = { he01->Origin(), nullptr, he10->Origin(), nullptr };
			array<size_t, 4> indices = { heMesh->Index(vertices[0]), static_cast<size_t>(-1), heMesh->Index(vertices[2]), static_cast<size_t>(-1) };
			do
			{
				for (auto idx : sortedIndices)
					vertexMutexes[idx].unlock();

				sortedIndices.clear();
				vertices[1] = he10->Next()->End();
				vertices[3] = he01->Next()->End();
				indices[1] = heMesh->Index(vertices[1]);
				indices[3] = heMesh->Index(vertices[3]);
				for (auto idx : indices)
					sortedIndices.insert(idx);

				for (auto idx : sortedIndices)
					vertexMutexes[idx].lock();
			} while (vertices[1] != he10->Next()->End() || vertices[3] != he01->Next()->End());

			// kernel
			do // do ... while(false) trick;
			{
				array<int, 4> degrees;
				degrees[0] = static_cast<int>(vertices[0]->Degree());
				if (degrees[0] <= 3)
					break;
				degrees[2] = static_cast<int>(vertices[2]->Degree());
				if (degrees[2] <= 3)
					break;
				degrees[1] = static_cast<int>(vertices[1]->Degree());
				degrees[3] = static_cast<int>(vertices[3]->Degree());

				int sumCost = 0;
				int sumFlipedCost = 0;
				for (size_t i = 0; i < 4; i++) {
					int diff = degrees[i] - (vertices[i]->IsBoundary() ? 4 : 6);
					int flipedDiff = diff + (i < 2 ? -1 : 1);
					sumCost += diff * diff;
					sumFlipedCost += flipedDiff * flipedDiff;
				}

				if (sumFlipedCost >= sumCost)
					break;

				vector<pointf3> positions;
				for (auto v : vertices)
					positions.push_back(v->pos.cast_to<pointf3>());
				if (!Geometry::IsConvexPolygon(positions))
					break;

				heMesh->FlipEdge(e);
			} while (false);

			for (auto idx : sortedIndices)
				vertexMutexes[idx].unlock();
		};
		Parallel::Instance().Run(step4, heMesh->Edges());

		if (heMesh->NumPolygons() == 2)
			break; // dihedron

		// 5. vertex normal
		printf("5. vertex normal\n");
		vector<vecf3> triWNs(heMesh->NumPolygons(), vecf3(0.f)); // triangle weighted normals
		vector<float> triAreas(heMesh->NumPolygons(), 0.f); // triangle areas
		vector<vecf3> sWNs(heMesh->NumVertices(), vecf3(0.f)); // sum Weighted Normal
		auto step5_tri = [&](HEMesh<V>::P* triangle) {
			auto vertices = triangle->BoundaryVertice();
			assert(vertices.size() == 3);

			auto d10 = vertices[0]->pos - vertices[1]->pos;
			auto d12 = vertices[2]->pos - vertices[1]->pos;
			auto wN = d12.cross(d10);

			size_t idx = heMesh->Index(triangle);
			triWNs[idx] = wN;
			triAreas[idx] = wN.norm();
		};
		auto step5_v = [&](V* v) {
			size_t idx = heMesh->Index(v);
			assert((v->IsBoundary() && v->AdjPolygons().size() >= 2)
				|| (!v->IsBoundary() && v->AdjPolygons().size() >= 3));
			for (auto adjP : v->AdjPolygons()) {
				if (HEMesh<V>::P::IsBoundary(adjP))
					continue;
				sWNs[idx] += triWNs[heMesh->Index(adjP)];
			}
		};
		Parallel::Instance().Run(step5_tri, heMesh->Polygons());
		Parallel::Instance().Run(step5_v, heMesh->Vertices());

		// 6. tangential smoothing
		printf("6. tangential smoothing\n");
		constexpr float w = 0.2f;// avoid oscillation
		auto step6 = [&](V* v) {
			if (v->IsBoundary()) {
				v->newPos = v->pos;
				return;
			}

			// gravity-weighted offset
			vecf3 gravityCentroid{ 0.f };
			float sumArea = 0.f;
			for (auto outHE : v->OutHEs()) {
				auto p0 = outHE->Polygon();
				auto  p1 = outHE->Pair()->Polygon();
				float area = 0.5f * (triAreas[heMesh->Index(p0)] + triAreas[heMesh->Index(p1)]);

				sumArea += area;
				gravityCentroid += area * outHE->End()->pos;
			}
			gravityCentroid /= sumArea;
			vecf3 offset = gravityCentroid - v->pos;

			// normal
			vecf3 normal = sWNs[heMesh->Index(v)].normalize();

			// tangent offset
			vecf3 tangentOffset = offset - offset.dot(normal) * normal;
			auto newPos = v->pos + w * tangentOffset;

			// project back
			v->newPos = v->Project(newPos, normal.cast_to<normalf>());
		};
		Parallel::Instance().Run(step6, heMesh->Vertices());

		// 7. update pos
		printf("7. update pos\n");
		auto step7 = [](V* v) { assert(!v->newPos.has_nan()); v->pos = v->newPos; };
		Parallel::Instance().Run(step7, heMesh->Vertices());
	}

	return true;
}

const vecf3 IsotropicRemeshing::V::Project(const vecf3& p, const normalf& norm) const {
	rayf3 ray(p.cast_to<pointf3>(), -norm.cast_to<vecf3>());
	const auto adjVs = AdjVertices();
	for (size_t i = 0; i < adjVs.size(); i++) {
		size_t next = (i + 1) % adjVs.size();
		auto rst = ray.intersect_triangle(pos.cast_to<pointf3>(), adjVs[i]->pos.cast_to<pointf3>(), adjVs[next]->pos.cast_to<pointf3>());
		if (get<0>(rst)) // isIntersect
			return ray.at(get<2>(rst)).cast_to<vecf3>();
	}
	return p;
}

bool IsotropicRemeshing::E::IsCanCollapse(float min, float maxL) const {
	if (Length() > min)
		return false;

	auto p0 = HalfEdge()->Origin();
	auto p1 = HalfEdge()->End();

	if (p0->IsBoundary() || p1->IsBoundary())
		return false;

	for (auto adjV : p0->AdjVertices()) {
		if (adjV->IsBoundary())
			return false;
	}

	for (auto adjV : p1->AdjVertices()) {
		if (adjV->IsBoundary())
			return false;
	}

	const auto c = Centroid();
	for (auto adjV : AdjVertices()) {
		if (vecf3::distance(adjV->pos, c) > maxL)
			return false;
	}

	return true;
}