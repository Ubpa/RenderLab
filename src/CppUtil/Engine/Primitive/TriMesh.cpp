#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/Triangle.h>

#include <CppUtil/Basic/Cube.h>
#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>
#include <CppUtil/Basic/Disk.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

TriMesh::TriMesh(const vector<uint> & indice,
	const vector<Point3> & positions,
	const vector<Normalf> & normals,
	const vector<Point2> & texcoords,
	const vector<Normalf> & tangents,
	ENUM_TYPE type)
:	indice(indice),
	positions(positions),
	normals(normals),
	texcoords(texcoords),
	tangents(tangents),
	type(type)
{
	if (!(indice.size() > 0 && indice.size() % 3 == 0)
		|| positions.size() <= 0
		|| normals.size() != positions.size()
		|| texcoords.size() != positions.size()
		|| (tangents.size() != 0 && tangents.size() != positions.size())) {
		type = ENUM_TYPE::INVALID;
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (uint i = 0; i < indice.size(); i += 3)
		triangles.push_back(Triangle::New(indice[i], indice[i+1], indice[i+2]));

	if(tangents.size() == 0)
		GenTangents();
}

TriMesh::TriMesh(uint triNum, uint vertexNum,
	const uint * indice,
	const float * positions,
	const float * normals,
	const float * texcoords,
	const float * tangents,
	ENUM_TYPE type)
	: type(type)
{
	if (!indice || !positions || !normals || !texcoords) {
		type = ENUM_TYPE::INVALID;
		printf("ERROR: TriMesh is invalid.\n");
		return;
	}

	for (uint i = 0; i < vertexNum; i++) {
		this->positions.push_back(Point3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]));
		this->normals.push_back(Normalf(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]));
		this->texcoords.push_back(Point2(texcoords[2 * i], texcoords[2 * i + 1]));
		if(tangents)
			this->tangents.push_back({ tangents[3 * i],tangents[3 * i + 1],tangents[3 * i + 2] });
	}

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

void TriMesh::Init_AfterGenPtr() {
	auto triMesh = This<TriMesh>();
	for (auto triangle : triangles)
		triangle->mesh = triMesh;

	for (auto triangle : triangles)
		box.UnionWith(triangle->GetBBox());
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

		float r = 1.0F / (s1 * t2 - s2 * t1);
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
		tangents[a] = (t - n * n.Dot(t)).Normalize();

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
