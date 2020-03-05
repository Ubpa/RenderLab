#include <Engine/Triangle.h>

#include <Engine/TriMesh.h>
#include <UGM/bbox.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

const Ubpa::bboxf3 Triangle::GetBBox() const {
	const auto & positions = mesh.lock()->GetPositions();

	Ubpa::pointf3 pArr[3] = {
		positions[idx[0]],
		positions[idx[1]],
		positions[idx[2]]
	};

	Ubpa::pointf3 minP = Ubpa::pointf3::min(Ubpa::pointf3::min(pArr[0],pArr[1]),pArr[2]);
	Ubpa::pointf3 maxP = Ubpa::pointf3::max(Ubpa::pointf3::max(pArr[0], pArr[1]), pArr[2]);

	for (int dim = 0; dim < 3; dim++) {
		if (minP[dim] == maxP[dim]) {
			minP[dim] -= 0.001f;
			maxP[dim] += 0.001f;
		}
	}

	return Ubpa::bboxf3(minP, maxP);
}

float Triangle::GetArea() const {
	const auto & positions = mesh.lock()->GetPositions();
	const auto e1 = positions[idx[1]] - positions[idx[0]];
	const auto e2 = positions[idx[2]] - positions[idx[0]];

	return 0.5f * e1.cross(e2).norm();
}
