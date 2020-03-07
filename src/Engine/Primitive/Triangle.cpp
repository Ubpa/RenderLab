#include <Engine/Primitive/Triangle.h>

#include <Engine/Primitive/TriMesh.h>
#include <UGM/bbox.h>

using namespace Ubpa;

const bboxf3 Triangle::GetBBox() const {
	const auto & positions = mesh.lock()->GetPositions();

	pointf3 pArr[3] = {
		positions[idx[0]],
		positions[idx[1]],
		positions[idx[2]]
	};

	pointf3 minP = pointf3::min(pointf3::min(pArr[0],pArr[1]),pArr[2]);
	pointf3 maxP = pointf3::max(pointf3::max(pArr[0], pArr[1]), pArr[2]);

	for (int dim = 0; dim < 3; dim++) {
		if (minP[dim] == maxP[dim]) {
			minP[dim] -= 0.001f;
			maxP[dim] += 0.001f;
		}
	}

	return bboxf3(minP, maxP);
}

float Triangle::GetArea() const {
	const auto & positions = mesh.lock()->GetPositions();
	const auto e1 = positions[idx[1]] - positions[idx[0]];
	const auto e2 = positions[idx[2]] - positions[idx[0]];

	return 0.5f * e1.cross(e2).norm();
}
