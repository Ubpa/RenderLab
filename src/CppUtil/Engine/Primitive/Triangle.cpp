#include <CppUtil/Engine/Triangle.h>
#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil::Engine;
using namespace glm;

BBox Triangle::GetBBox() const {
	auto positions = mesh.lock()->GetPositions();

	vec3 pArr[3] = {
		positions[idx[0]],
		positions[idx[1]],
		positions[idx[2]]
	};

	vec3 minP = min(min(pArr[0], pArr[1]), pArr[2]);
	vec3 maxP = max(max(pArr[0], pArr[1]), pArr[2]);

	for (size_t dim = 0; dim < 3; dim++) {
		if (minP[dim] == maxP[dim]) {
			minP[dim] -= 0.000001;
			maxP[dim] += 0.000001;
		}
	}

	return { minP, maxP };
}
