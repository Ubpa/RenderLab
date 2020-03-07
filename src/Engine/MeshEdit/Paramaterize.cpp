#include <Engine/MeshEdit/Paramaterize.h>

#include <Engine/MeshEdit/MinSurf.h>

#include <Engine/Primitive/TriMesh.h>

using namespace Ubpa;

using namespace std;

Paramaterize::Paramaterize(Ptr<TriMesh> triMesh)
	: minSurf(MinSurf::New(nullptr))
{
	Init(triMesh);
}

void Paramaterize::Clear() {
	minSurf->Clear();
}

bool Paramaterize::Init(Ptr<TriMesh> triMesh) {
	auto success = minSurf->Init(triMesh);
	if(!success) {
		minSurf->Clear();
		return false;
	}

	auto numBound = minSurf->heMesh->NumBoundaries();
	if (numBound == 0 || numBound > 1) {
		printf("ERROR::Paramaterize::Init:\n"
			"\t""numBound(%zd) : numBound == 0 || numBound > 1\n", numBound);
		minSurf->Clear();
		return false;
	}

	return true;
}

bool Paramaterize::Run() {
	if (minSurf->heMesh->IsEmpty() || !minSurf->triMesh) {
		printf("ERROR::Paramaterize::Run:\n"
			"\t""minSurf->heMesh->IsEmpty() || !minSurf->triMesh\n");
		return false;
	}

	SetBoundPos();

	minSurf->Minimize();

	if (!minSurf->heMesh->IsTriMesh() || minSurf->heMesh->NumBoundaries() != 1) {
		printf("ERROR::LoopSubdivision::Run\n"
			"\t""!minSurf->heMesh->IsTriMesh() || minSurf->heMesh->NumBoundaries() != 1, algorithm error\n");
		return false;
	}

	vector<pointf2> texcoords;
	// order remain, so we can directly get correct order
	for (auto v : minSurf->heMesh->Vertices())
		texcoords.push_back({ v->pos[0],v->pos[1] });

	minSurf->triMesh->Update(texcoords);

	return true;
}

void Paramaterize::SetBoundPos() {
	// get boundary
	auto boundary = minSurf->heMesh->Boundaries()[0];
	auto n = boundary.size();
	vector<MinSurf::V*> vertices;
	for (auto he : boundary)
		vertices.push_back(he->Origin());

	// compute dis
	vector<float> disVec;
	float sumDis = 0.f;
	for (size_t i = 0; i < n; i++) {
		auto next = (i + 1) % n;
		auto pos0 = vertices[i]->pos;
		auto pos1 = vertices[next]->pos;
		float dis = (pos1 - pos0).norm();
		disVec.push_back(dis);
		sumDis += dis;
	}

	// set pos according dis
	float accDis = 0.f;
	for (size_t i = 0; i < n; i++) {
		float s = accDis / sumDis;

		pointf2 pos;
		if (s < 0.25f) {
			float t = 4.f * s;
			pos = pointf2(0.f, t);
		}
		else if (s < 0.50f) {
			float t = 4.f * (s - 0.25f);
			pos = pointf2(t, 1.f);
		}
		else if (s < 0.75f) {
			float t = 4.f * (s - 0.50f);
			pos = pointf2(1.f, 1.f - t);
		}
		else {
			float t = 4.f * (s - 0.75f);
			pos = pointf2(1.f - t, 0.f);
		}

		vertices[i]->pos = vecf3(pos[0], pos[1], 0.f);

		accDis += disVec[i];
	}
}
