#include <Engine/Primitive/TriMesh.h>

#include <Engine/Primitive/Triangle.h>

#include <Basic/ShapeMesh/CubeMesh.h>
#include <Basic/ShapeMesh/SphereMesh.h>
#include <Basic/ShapeMesh/PlaneMesh.h>
#include <Basic/ShapeMesh/DiskMesh.h>
#include <Basic/Sampler/BasicSampler.h>
#include <Basic/Parallel.h>

#include <mutex>
#include <map>

using namespace Ubpa;

using namespace std;
using namespace Ubpa;

TriMesh::TriMesh(unsigned triNum, unsigned vertexNum,
	const unsigned * indice,
	const float * positions,
	const float * normals,
	const float * texcoords,
	const float * tangents,
	ENUM_TYPE type)
	: type(type)
{
	if (!indice || !positions || !texcoords) {
		type = ENUM_TYPE::INVALID;
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}

	for (unsigned i = 0; i < vertexNum; i++) {
		this->positions.push_back(pointf3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]));
		if(normals)
			this->normals.push_back(normalf(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]));
		if(texcoords)
			this->texcoords.push_back(pointf2(texcoords[2 * i], texcoords[2 * i + 1]));
		if(tangents)
			this->tangents.push_back({ tangents[3 * i],tangents[3 * i + 1],tangents[3 * i + 2] });
	}

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (unsigned i = 0; i < triNum; i++) {
		this->indice.push_back(indice[3 * i]);
		this->indice.push_back(indice[3 * i + 1]);
		this->indice.push_back(indice[3 * i + 2]);

		triangles.push_back(Triangle::New(indice[3 * i], indice[3 * i + 1], indice[3 * i + 2]));
	}

	if (!texcoords)
		this->texcoords.resize(vertexNum);

	if (!normals)
		GenNormals();

	if (!tangents) {
		if (texcoords)
			GenTangents();
		else
			this->tangents.resize(vertexNum);
	}
}

void TriMesh::Init(bool creator, const std::vector<unsigned> & indice,
	const std::vector<pointf3> & positions,
	const std::vector<normalf> & normals,
	const std::vector<pointf2> & texcoords,
	const std::vector<normalf> & tangents,
	ENUM_TYPE type)
{
	this->indice.clear();
	this->positions.clear();
	this->normals.clear();
	this->texcoords.clear();
	triangles.clear();
	this->type = ENUM_TYPE::INVALID;

	if (!(indice.size() > 0 && indice.size() % 3 == 0)
		|| positions.size() <= 0
		|| !normals.empty() && normals.size() != positions.size()
		|| !texcoords.empty() && texcoords.size() != positions.size()
		|| (tangents.size() != 0 && tangents.size() != positions.size()))
	{
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}

	this->indice = indice;
	this->positions = positions;
	this->type = type;

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (size_t i = 0; i < indice.size(); i += 3)
		triangles.push_back(Triangle::New(indice[i], indice[i + 1], indice[i + 2]));

	if (texcoords.empty())
		this->texcoords.resize(positions.size());
	else
		this->texcoords = texcoords;

	if (normals.empty())
		GenNormals();
	else
		this->normals = normals;

	if (tangents.size() == 0) {
		if (texcoords.empty())
			this->tangents.resize(positions.size());
		else
			GenTangents();
	}
	else
		this->tangents = tangents;

	if (!creator)
		Init_AfterGenPtr();
}

bool TriMesh::Update(const std::vector<pointf3> & positions) {
	if (type == INVALID) {
		printf("ERROR::TriMesh::Update:\n"
			"\t""type == INVALID\n");
		return false;
	}

	if (positions.size() != this->positions.size()) {
		printf("ERROR::TriMesh::Update:\n"
			"\t""%zd positions.size() != %zd this->positions.size()\n", positions.size(), this->positions.size());
		return false;
	}

	this->positions = positions;

	return true;
}

bool TriMesh::Update(const vector<pointf2> & texcoords) {
	if (type == INVALID) {
		printf("ERROR::TriMesh::Update:\n"
			"\t""type == INVALID\n");
		return false;
	}

	if (texcoords.size() != positions.size()) {
		printf("ERROR::TriMesh::Update:\n"
			"\t""%zd texcoords.size() != %zd positions.size()\n", texcoords.size(), positions.size());
		return false;
	}

	this->texcoords = texcoords;

	return true;
}

void TriMesh::Init_AfterGenPtr() {
	auto triMesh = This<TriMesh>();
	for (auto triangle : triangles)
		triangle->mesh = triMesh;

	for (auto triangle : triangles)
		box.combine_with(triangle->GetBBox());
}

void TriMesh::GenNormals() {
	normals.clear();
	normals.resize(positions.size(), normalf(0.f));

	vector<mutex> vertexMutexes(positions.size());
	auto calSWN = [&](Ptr<Triangle> triangle) {
		auto v0 = triangle->idx[0];
		auto v1 = triangle->idx[1];
		auto v2 = triangle->idx[2];
		
		auto d10 = positions[v0] - positions[v1];
		auto d12 = positions[v2] - positions[v1];
		auto wN = d12.cross(d10);
		
		for (size_t i = 0; i < 3; i++) {
			auto v = triangle->idx[i];
			vertexMutexes[v].lock();
			normals[v] += wN.cast_to<normalf>();
			vertexMutexes[v].unlock();
		}
	};
	auto calN = [&](size_t v) { normals[v].normalize_self(); };
	Parallel::Instance().Run(calSWN, triangles);
	Parallel::Instance().Run(calN, positions.size());
}

void TriMesh::GenTangents() {
	const size_t vertexNum = positions.size();
	const size_t triangleCount = indice.size() / 3;

	vector<normalf> tanS(vertexNum);
	vector<normalf> tanT(vertexNum);
	vector<mutex> vertexMutexes(vertexNum);
	auto calST = [&](Ptr<Triangle> triangle) {
		auto i1 = triangle->idx[0];
		auto i2 = triangle->idx[1];
		auto i3 = triangle->idx[2];

		const pointf3& v1 = positions[i1];
		const pointf3& v2 = positions[i2];
		const pointf3& v3 = positions[i3];

		const pointf2& w1 = texcoords[i1];
		const pointf2& w2 = texcoords[i2];
		const pointf2& w3 = texcoords[i3];

		float x1 = v2[0] - v1[0];
		float x2 = v3[0] - v1[0];
		float y1 = v2[1] - v1[1];
		float y2 = v3[1] - v1[1];
		float z1 = v2[2] - v1[2];
		float z2 = v3[2] - v1[2];

		float s1 = w2[0] - w1[0];
		float s2 = w3[0] - w1[0];
		float t1 = w2[1] - w1[1];
		float t2 = w3[1] - w1[1];

		float denominator = s1 * t2 - s2 * t1;
		float r = denominator == 0.f ? 1.f : 1.f / denominator;
		normalf sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		normalf tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		for (size_t i = 0; i < 3; i++) {
			auto v = triangle->idx[i];
			vertexMutexes[v].lock();
			tanS[v] += sdir;
			tanT[v] += tdir;
			vertexMutexes[v].unlock();
		}
	};
	Parallel::Instance().Run(calST, triangles);

	tangents.resize(vertexNum);
	auto calTan = [&](size_t i) {
		const normalf& n = normals[i];
		const normalf& t = tanS[i];

		// Gram-Schmidt orthogonalize
		auto projT = t - n * n.dot(t);
		tangents[i] = projT.norm2() == 0.f ? BasicSampler::UniformOnSphere().cast_to<normalf>() : projT.normalize();

		// Calculate handedness
		tangents[i] *= (n.cross(t).dot(tanT[i]) < 0.0F) ? -1.0F : 1.0F;
	};
	Parallel::Instance().Run(calTan, vertexNum);
}

const Ptr<TriMesh> TriMesh::GenCube() {
	CubeMesh cube;
	auto cubeMesh = TriMesh::New(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr(), cube.GetTexCoordsArr(), nullptr, ENUM_TYPE::CUBE);
	return cubeMesh;
}

const Ptr<TriMesh> TriMesh::GenSphere() {
	SphereMesh sphere(50);
	auto sphereMesh = TriMesh::New(sphere.GetTriNum(), sphere.GetVertexNum(),
		sphere.GetIndexArr(), sphere.GetPosArr(), sphere.GetNormalArr(), sphere.GetTexCoordsArr(), sphere.GetTangentArr(), ENUM_TYPE::SPHERE);
	return sphereMesh;
}

const Ptr<TriMesh> TriMesh::GenPlane() {
	PlaneMesh plane;
	auto planeMesh = TriMesh::New(plane.GetTriNum(), plane.GetVertexNum(),
		plane.GetIndexArr(), plane.GetPosArr(), plane.GetNormalArr(), plane.GetTexCoordsArr(), nullptr, ENUM_TYPE::PLANE);
	return planeMesh;
}

const Ptr<TriMesh> TriMesh::GenDisk() {
	DiskMesh disk(50);
	auto diskMesh = TriMesh::New(disk.GetTriNum(), disk.GetVertexNum(),
		disk.GetIndexArr(), disk.GetPosArr(), disk.GetNormalArr(), disk.GetTexCoordsArr(), disk.GetTangentArr(), ENUM_TYPE::DISK);
	return diskMesh;
}
