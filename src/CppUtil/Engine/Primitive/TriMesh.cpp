#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/BVHNode.h>

#include <CppUtil/Basic/Cube.h>
#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>

#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

TriMesh::TriMesh(const std::vector<uint> & indice,
	const std::vector<glm::vec3> & positions,
	const std::vector<glm::vec3> & normals,
	const std::vector<glm::vec2> & texcoords)
:	indice(indice),
	positions(positions),
	normals(normals),
	texcoords(texcoords)
{
	assert(indice.size() > 0 && indice.size() % 3 == 0);
	assert(positions.size() > 0);
	assert(normals.size() == positions.size());
	assert(texcoords.size() == positions.size());

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (uint i = 0; i < indice.size(); i += 3)
		triangles.push_back(ToPtr(new Triangle(indice[i], indice[i+1], indice[i+2])));

	GenTangents();
}

TriMesh::TriMesh(uint triNum, uint vertexNum,
	const uint * indice,
	const float * positions,
	const float * normals,
	const float * texcoords)
{
	assert(indice != nullptr);
	assert(positions != nullptr);
	assert(normals != nullptr);
	assert(texcoords != nullptr);

	for (uint i = 0; i < vertexNum; i++) {
		this->positions.push_back(vec3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]));
		this->normals.push_back(vec3(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]));
		this->texcoords.push_back(vec2(texcoords[2 * i], texcoords[2 * i + 1]));
	}

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (uint i = 0; i < triNum; i++) {
		this->indice.push_back(indice[3 * i]);
		this->indice.push_back(indice[3 * i + 1]);
		this->indice.push_back(indice[3 * i + 2]);

		triangles.push_back(ToPtr(new Triangle(indice[3 * i], indice[3 * i + 1], indice[3 * i + 2])));
	}

	GenTangents();
}

void TriMesh::InitAfterGenSharePtr() {
	for (auto triangle : triangles)
		triangle->mesh = This();
}

void TriMesh::GenTangents() {
	int vertexCount = positions.size();
	int triangleCount = indice.size() / 3;

	vec3 *tan1 = new vec3[vertexCount * 2]();
	vec3 *tan2 = tan1 + vertexCount;

	for (int a = 0; a < triangleCount; a++)
	{
		uint i1 = indice[3 * a];
		uint i2 = indice[3 * a + 1];
		uint i3 = indice[3 * a + 2];

		const vec3 & v1 = positions[i1];
		const vec3 & v2 = positions[i2];
		const vec3 & v3 = positions[i3];

		const vec2 & w1 = texcoords[i1];
		const vec2 & w2 = texcoords[i2];
		const vec2 & w3 = texcoords[i3];

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
		vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	tangents.resize(vertexCount);
	for (int a = 0; a < vertexCount; a++)
	{
		const vec3 & n = normals[a];
		const vec3 & t = tan1[a];

		// Gram-Schmidt orthogonalize
		tangents[a] = normalize(t - n * dot(n, t));

		// Calculate handedness
		tangents[a] *= (dot(cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}

TriMesh::Ptr TriMesh::GenCube() {
	Cube cube;
	auto cubeMesh = ToPtr(new TriMesh(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr(), cube.GetTexCoordsArr()));
	return cubeMesh;
}

TriMesh::Ptr TriMesh::GenSphere() {
	Sphere sphere(50);
	auto sphereMesh = ToPtr(new TriMesh(sphere.GetTriNum(), sphere.GetVertexNum(),
		sphere.GetIndexArr(), sphere.GetPosArr(), sphere.GetNormalArr(), sphere.GetTexCoordsArr()));
	return sphereMesh;
}

TriMesh::Ptr TriMesh::GenPlane() {
	Plane plane;
	auto planeMesh = ToPtr(new TriMesh(plane.GetTriNum(), plane.GetVertexNum(),
		plane.GetIndexArr(), plane.GetPosArr(), plane.GetNormalArr(), plane.GetTexCoordsArr()));
	return planeMesh;
}