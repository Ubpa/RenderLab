#include <Engine/MeshEdit/DeformRBF.h>

#include <Engine/Primitive/TriMesh.h>

#include <Eigen/Dense>

using namespace Ubpa;

using namespace std;
using namespace Eigen;

void DeformRBF::Clear() {
	triMesh = nullptr;
	origPos.clear();
}

bool DeformRBF::Init(Ptr<TriMesh> triMesh) {
	if (triMesh == this->triMesh)
		return true;

	if (triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::DeformRBF::Init\n"
			"\t""triMesh->GetType() == TriMesh::INVALID\n");
		return false;
	}

	this->triMesh = triMesh;
	origPos = triMesh->GetPositions();
	return true;
}

bool DeformRBF::Run(const std::vector<Constraint> & constraints, const vector<size_t> & updateIndice) {
	if (!triMesh) {
		printf("ERROR::DeformRBF::Run\n"
			"\t""!triMesh\n");
		return false;
	}

	for (auto constraint : constraints) {
		if (constraint.id >= origPos.size()) {
			printf("ERROR::DeformRBF::Run\n"
				"\t""constraint.id >= origPos.size()\n");
			return false;
		}
	}

	// compute RBF matrix
	auto phi = [](float r) -> float {return r*r*r; }; // basis function
	size_t m = constraints.size();
	size_t k = 10;
	
	// A = -       -
	//     | G   P |
	//     |       |
	//     |  t    |
	//     | P   0 |
	//     -       -
	MatrixXf A(m + k, m + k);
	
	// G : m x m
	// Gij = gj(ci) = basicFunc(||ci-cj||)
	for (size_t i = 0; i < m; i++) {
		for (size_t j = 0; j < m; j++) {
			auto ci = origPos[constraints[i].id];
			auto cj = origPos[constraints[j].id];
			float norm = pointf3::distance(ci, cj);
			A(i, j) = phi(norm);
		}
	}

	// P : m x k
	// Pij = pj(ci)
	// [p0, p1, ..., p9] is a basis of the space of trivariate quadratic polunomials
	// e[1]. [1, xx, xy, xz, yx, yy, yz, zx, zy, zz]
	for (size_t i = 0; i < m; i++) {
		auto ci = origPos[constraints[i].id];
		A(m + 0, i) = A(i, m + 0) = 1.f;
		A(m + 1, i) = A(i, m + 1) = ci[0] * ci[0];
		A(m + 2, i) = A(i, m + 2) = ci[0] * ci[1];
		A(m + 3, i) = A(i, m + 3) = ci[0] * ci[2];
		A(m + 4, i) = A(i, m + 4) = ci[1] * ci[0];
		A(m + 5, i) = A(i, m + 5) = ci[1] * ci[1];
		A(m + 6, i) = A(i, m + 6) = ci[1] * ci[2];
		A(m + 7, i) = A(i, m + 7) = ci[2] * ci[0];
		A(m + 8, i) = A(i, m + 8) = ci[2] * ci[1];
		A(m + 9, i) = A(i, m + 9) = ci[2] * ci[2];
	}

	// 0 : k x k
	for (size_t i = 0; i < k; i++) {
		for (size_t j = 0; j < k; j++) {
			A(m + i, m + j) = 0.f;
		}
	}

	MatrixXf b(m + k, 3);
	for (int i = 0; i < k; i++)
		b(m + i,0) = b(m + i, 1) = b(m + i, 2) = 0;
	for (int i = 0; i < m; i++) {
		auto bi = constraints[i].pos;
		b(i, 0) = bi[0];
		b(i, 1) = bi[1];
		b(i, 2) = bi[2];
	}

	MatrixXf RBF = A.colPivHouseholderQr().solve(b);

	// compute target pos of vertex
	vector<pointf3> qVec;
	for (auto id : updateIndice) {
		auto pos = origPos[id];
		// compute p
		MatrixXf p(1, m + k);
		for (size_t i = 0; i < m; i++) {
			auto ci = origPos[constraints[i].id];
			p(i) = phi(pointf3::distance(ci, pos));
		}
		p(0, m + 0) = 1;
		p(0, m + 1) = pos[0] * pos[0];
		p(0, m + 2) = pos[0] * pos[1];
		p(0, m + 3) = pos[0] * pos[2];
		p(0, m + 4) = pos[1] * pos[0];
		p(0, m + 5) = pos[1] * pos[1];
		p(0, m + 6) = pos[1] * pos[2];
		p(0, m + 7) = pos[2] * pos[0];
		p(0, m + 8) = pos[2] * pos[1];
		p(0, m + 9) = pos[2] * pos[2];

		// compute q
		MatrixXf q = p * RBF;
		triMesh->GetPositions()[id] = { q(0,0),q(0,1),q(0,2) };
	}

	return true;
}

void DeformRBF::Restore() {
	if (triMesh == nullptr)
		return;

	triMesh->Update(origPos);
}
