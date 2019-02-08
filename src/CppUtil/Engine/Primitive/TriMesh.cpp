#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

TriMesh::TriMesh(const std::vector<size_t> & indice,
	const std::vector<glm::vec3> & positions,
	const std::vector<glm::vec3> & normals,
	const std::vector<glm::vec3> & texcoords)
:	positions(positions),
	normals(normals),
	texcoords(texcoords),
	bvhRoot(nullptr)
{
	assert(indice.size() % 3 == 0);
	assert(positions.size() > 0);
	assert(normals.size() == positions.size());
	assert(texcoords.size() == 0 || texcoords.size() == positions.size());

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (size_t i = 0; i < indice.size(); i += 3)
		triangles.push_back(ToPtr(new Triangle(indice[i], indice[i+1], indice[i+2])));
}

TriMesh::TriMesh(size_t triNum, size_t vertexNum,
	const size_t * indice,
	const float * positions,
	const float * normals,
	const float * texcoords)
{
	assert(indice != nullptr);
	assert(positions != nullptr);
	assert(normals != nullptr);

	for (size_t i = 0; i < vertexNum; i++) {
		this->positions.push_back(vec3(positions[3 * i], positions[3 * i + 1], positions[3 * i + 2]));
		this->normals.push_back(vec3(normals[3 * i], normals[3 * i + 1], normals[3 * i + 2]));
		if (texcoords != nullptr)
			this->texcoords.push_back(vec3(texcoords[3 * i], texcoords[3 * i + 1], texcoords[3 * i + 2]));
	}

	// traingel 的 mesh 在 init 的时候设置
	// 因为现在还没有生成 share_ptr
	for (size_t i = 0; i < triNum; i++)
		triangles.push_back(ToPtr(new Triangle(indice[3 * i], indice[3 * i + 1], indice[3 * i + 2])));
}

void TriMesh::InitAfterGenSharePtr() {
	for (auto triangle : triangles)
		triangle->mesh = This();

	bvhRoot = ToPtr(new BVHNode<Triangle>(triangles, 4, 0, triangles.size()));
}
