#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/Triangle.h>

#include <CppUtil/Basic/Cube.h>
#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>
#include <CppUtil/Basic/Disk.h>
#include <CppUtil/Basic/BasicSampler.h>

#include <map>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

TriMesh::TriMesh(uint triNum, uint vertexNum,
	const uint * indice,
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

	for (uint i = 0; i < vertexNum; i++) {
		this->positions.push_back(Point3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]));
		if(normals)
			this->normals.push_back(Normalf(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]));
		if(texcoords)
			this->texcoords.push_back(Point2(texcoords[2 * i], texcoords[2 * i + 1]));
		if(tangents)
			this->tangents.push_back({ tangents[3 * i],tangents[3 * i + 1],tangents[3 * i + 2] });
	}

	if (!normals)
		GenNormals();
	if (!texcoords)
		this->texcoords.resize(vertexNum);

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (uint i = 0; i < triNum; i++) {
		this->indice.push_back(indice[3 * i]);
		this->indice.push_back(indice[3 * i + 1]);
		this->indice.push_back(indice[3 * i + 2]);

		triangles.push_back(Triangle::New(indice[3 * i], indice[3 * i + 1], indice[3 * i + 2]));
	}

	if(!tangents)
		GenTangents();
}

void TriMesh::Init(bool creator, const std::vector<uint> & indice,
	const std::vector<Point3> & positions,
	const std::vector<Normalf> & normals,
	const std::vector<Point2> & texcoords,
	const std::vector<Normalf> & tangents,
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

	if (normals.empty())
		GenNormals();
	else
		this->normals = normals;

	if (texcoords.empty())
		this->texcoords.resize(positions.size());
	else
		this->texcoords = texcoords;

	if (tangents.size() == 0)
		GenTangents();
	else
		this->tangents = tangents;

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (uint i = 0; i < indice.size(); i += 3)
		triangles.push_back(Triangle::New(indice[i], indice[i + 1], indice[i + 2]));

	if (!creator)
		Init_AfterGenPtr();
}

bool TriMesh::Update(const vector<Point2> & texcoords) {
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
		box.UnionWith(triangle->GetBBox());
}

void TriMesh::GenNormals() {
	normals.clear();
	map<size_t, vector<Normalf>> map2wNVec; // map vertex idx to weighted normal vec
	for (size_t i = 0; i < indice.size(); i+=3) {
		auto v0 = indice[i];
		auto v1 = indice[i+1];
		auto v2 = indice[i+2];

		auto pos0 = positions[v0];
		auto pos1 = positions[v1];
		auto pos2 = positions[v2];

		auto d10 = pos0 - pos1;
		auto d12 = pos2 - pos1;
		auto wN = d12.Cross(d10);

		map2wNVec[v0].push_back(wN);
		map2wNVec[v1].push_back(wN);
		map2wNVec[v2].push_back(wN);
	}

	normals.resize(positions.size());
	for (auto pair : map2wNVec) {
		Normalf sumWN;
		float sumW = 0.f;
		for (auto wN : pair.second) {
			sumWN += wN;
			sumW += wN.Norm();
		}
		normals[pair.first] = sumWN / sumW;
	}
}

void TriMesh::GenTangents() {
	size_t vertexCount = positions.size();
	size_t triangleCount = indice.size() / 3;

	Normalf *tan1 = new Normalf[vertexCount * 2]();
	Normalf *tan2 = tan1 + vertexCount;

	for (size_t a = 0; a < triangleCount; a++)
	{
		uint i1 = indice[3 * a];
		uint i2 = indice[3 * a + 1];
		uint i3 = indice[3 * a + 2];

		const Point3 & v1 = positions[i1];
		const Point3 & v2 = positions[i2];
		const Point3 & v3 = positions[i3];

		const Point2 & w1 = texcoords[i1];
		const Point2 & w2 = texcoords[i2];
		const Point2 & w3 = texcoords[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float denominator = s1 * t2 - s2 * t1;
		float r = denominator == 0.f ? 1.f : 1.f/denominator;
		Normalf sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		Normalf tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	tangents.resize(vertexCount);
	for (size_t a = 0; a < vertexCount; a++)
	{
		const Normalf & n = normals[a];
		const Normalf & t = tan1[a];

		// Gram-Schmidt orthogonalize
		auto projT = t - n * n.Dot(t);
		tangents[a] = projT.Norm2() == 0.f ? BasicSampler::UniformOnSphere() : projT.Normalize();

		// Calculate handedness
		tangents[a] *= (n.Cross(t).Dot(tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}

const Ptr<TriMesh> TriMesh::GenCube() {
	Cube cube;
	auto cubeMesh = TriMesh::New(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr(), cube.GetTexCoordsArr(), nullptr, ENUM_TYPE::CUBE);
	return cubeMesh;
}

const Ptr<TriMesh> TriMesh::GenSphere() {
	Sphere sphere(50);
	auto sphereMesh = TriMesh::New(sphere.GetTriNum(), sphere.GetVertexNum(),
		sphere.GetIndexArr(), sphere.GetPosArr(), sphere.GetNormalArr(), sphere.GetTexCoordsArr(), sphere.GetTangentArr(), ENUM_TYPE::SPHERE);
	return sphereMesh;
}

const Ptr<TriMesh> TriMesh::GenPlane() {
	Plane plane;
	auto planeMesh = TriMesh::New(plane.GetTriNum(), plane.GetVertexNum(),
		plane.GetIndexArr(), plane.GetPosArr(), plane.GetNormalArr(), plane.GetTexCoordsArr(), nullptr, ENUM_TYPE::PLANE);
	return planeMesh;
}

const Ptr<TriMesh> TriMesh::GenDisk() {
	Disk disk(50);
	auto diskMesh = TriMesh::New(disk.GetTriNum(), disk.GetVertexNum(),
		disk.GetIndexArr(), disk.GetPosArr(), disk.GetNormalArr(), disk.GetTexCoordsArr(), disk.GetTangentArr(), ENUM_TYPE::DISK);
	return diskMesh;
}
