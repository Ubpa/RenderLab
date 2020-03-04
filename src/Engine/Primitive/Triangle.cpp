#include <Engine/Triangle.h>

#include <Engine/TriMesh.h>
#include <Basic/UGM/BBox.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const BBoxf Triangle::GetBBox() const {
	const auto & positions = mesh.lock()->GetPositions();

	Point3 pArr[3] = {
		positions[idx[0]],
		positions[idx[1]],
		positions[idx[2]]
	};

	Point3 minP = pArr[0].MinWith(pArr[1]).MinWith(pArr[2]);
	Point3 maxP = pArr[0].MaxWith(pArr[1]).MaxWith(pArr[2]);

	for (int dim = 0; dim < 3; dim++) {
		if (minP[dim] == maxP[dim]) {
			minP[dim] -= 0.001f;
			maxP[dim] += 0.001f;
		}
	}

	return BBoxf(minP, maxP);
}

float Triangle::GetArea() const {
	const auto & positions = mesh.lock()->GetPositions();
	const auto e1 = positions[idx[1]] - positions[idx[0]];
	const auto e2 = positions[idx[2]] - positions[idx[0]];

	return 0.5f * e1.Cross(e2).Norm();
}
