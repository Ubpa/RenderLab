#include <CppUtil/Engine/MeshEdit/DeformRBF.h>

#include <CppUtil/Engine/TriMesh.h>

#include <Eigen/Dense>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;
using namespace Eigen;

void DeformRBF::Clear() {
	triMesh = nullptr;
	origPos.clear();
}

bool DeformRBF::Init(Basic::Ptr<TriMesh> triMesh) {
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

bool DeformRBF::Run(const std::vector<Constraint> & constraints) {
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
			float norm = Point3::Distance(ci, cj);
			A(i, j) = phi(norm);
		}
	}

	// P : m x k
	// Pij = pj(ci)
	// [p0, p1, ..., p9] is a basis of the space of trivariate quadratic polunomials
	// e.g. [1, xx, xy, xz, yx, yy, yz, zx, zy, zz]
	for (size_t i = 0; i < m; i++) {
		auto ci = origPos[constraints[i].id];
		A(m + 0, i) = A(i, m + 0) = 1.f;
		A(m + 1, i) = A(i, m + 1) = ci.x * ci.x;
		A(m + 2, i) = A(i, m + 2) = ci.x * ci.y;
		A(m + 3, i) = A(i, m + 3) = ci.x * ci.z;
		A(m + 4, i) = A(i, m + 4) = ci.y * ci.x;
		A(m + 5, i) = A(i, m + 5) = ci.y * ci.y;
		A(m + 6, i) = A(i, m + 6) = ci.y * ci.z;
		A(m + 7, i) = A(i, m + 7) = ci.z * ci.x;
		A(m + 8, i) = A(i, m + 8) = ci.z * ci.y;
		A(m + 9, i) = A(i, m + 9) = ci.z * ci.z;
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
		b(i, 0) = bi.x;
		b(i, 1) = bi.y;
		b(i, 2) = bi.z;
	}

	MatrixXf RBF = A.colPivHouseholderQr().solve(b);
	cout << A << endl << endl<< b<<endl << endl << RBF;

	// compute target pos of vertex
	vector<Point3> qVec;
	for (auto & pos : origPos) {
		// compute p
		MatrixXf p(1,m + k);
		for (size_t i = 0; i < m; i++) {
			auto ci = origPos[constraints[i].id];
			p(i) = phi(Point3::Distance(ci, pos));
		}
		p(0, m + 0) = 1;
		p(0, m + 1) = pos.x * pos.x;
		p(0, m + 2) = pos.x * pos.y;
		p(0, m + 3) = pos.x * pos.z;
		p(0, m + 4) = pos.y * pos.x;
		p(0, m + 5) = pos.y * pos.y;
		p(0, m + 6) = pos.y * pos.z;
		p(0, m + 7) = pos.z * pos.x;
		p(0, m + 8) = pos.z * pos.y;
		p(0, m + 9) = pos.z * pos.z;

		// compute q
		MatrixXf q = p * RBF;
		qVec.push_back({q(0,0),q(0,1),q(0,2)});
	}

	// update trimesh pos
	triMesh->Update(qVec);

	return true;
}

void DeformRBF::Restore() {
	if (triMesh == nullptr)
		return;

	triMesh->Update(origPos);
}
