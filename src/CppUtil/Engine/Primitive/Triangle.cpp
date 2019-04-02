#include <CppUtil/Engine/Triangle.h>

#include <CppUtil/Engine/TriMesh.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const BBoxf Triangle::GetBBox() const {
	auto positions = mesh.lock()->GetPositions();

	Point3 pArr[3] = {
		positions[idx[0]],
		positions[idx[1]],
		positions[idx[2]]
	};

	Point3 minP = pArr[0].MinWith(pArr[1]).MinWith(pArr[2]);
	Point3 maxP = pArr[0].MaxWith(pArr[1]).MaxWith(pArr[2]);

	for (int dim = 0; dim < 3; dim++) {
		if (minP[dim] == maxP[dim]) {
			minP[dim] -= 0.000001f;
			maxP[dim] += 0.000001f;
		}
	}

	return BBoxf(minP, maxP);
}
