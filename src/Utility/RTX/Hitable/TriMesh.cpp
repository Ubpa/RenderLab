#include <Utility/RTX/TriMesh.h>

#include <Utility/RTX/Triangle.h>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;
using namespace std;

TriMesh::TriMesh(const vector<Vertex> & vertexs, Material::CPtr material)
	: BVH_Node(material){
	if (vertexs.size() % 3 != 0) {
		isValid = false;
		return;
	}

	vector<Hitable::CPtr> triangles;
	for (size_t i = 0; i < vertexs.size(); i += 3) {
		auto triangle = ToPtr(new Triangle(vertexs[i], vertexs[i+1],vertexs[i+2]));
		triangles.push_back(triangle);
	}

	Build(triangles.begin(), triangles.end());
	isValid = true;
}