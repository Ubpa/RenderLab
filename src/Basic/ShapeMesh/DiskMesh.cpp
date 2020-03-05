#include <Basic/ShapeMesh/DiskMesh.h>

#include <Basic/Math.h>

#include <cmath>

using namespace Ubpa;

using namespace std;

DiskMesh::DiskMesh(unsigned n)
	: ShapeMesh(n+1, n)
{
	normalArr = vector<vecf3>(n + 1);
	texCoordsArr = vector<vecf2>(n + 1);
	tangentArr = vector<vecf3>(n + 1);
	indexArr = vector<valu3>(n);

	float deltaTheta = 2 * Math::PI / n;
	float y = 0;
	for (unsigned i = 0; i < n; i++) {
		float theta = i * deltaTheta;
		float z = cosf(theta);
		float x = sinf(theta);
		posArr[i] = vecf3(x, y, z);
		texCoordsArr[i] = vecf2((1 + x) / 2, (1 + z) / 2);
	}
	posArr[n] = vecf3(0,0,0);
	texCoordsArr[n] = vecf2(0.5, 0.5);

	for (unsigned i = 0; i < n + 1; i++) {
		normalArr[i] = vecf3(0,1,0);
		tangentArr[i] = vecf3(1,0,0);
	}

	for (unsigned i = 0; i < n; i++) {
		int next = (i + 1) % n;
		indexArr[i] = valu3(i,next,n);
	}
}

float * DiskMesh::GetNormalArr() {
	return normalArr.front().data();
}

float * DiskMesh::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

unsigned * DiskMesh::GetIndexArr() {
	return indexArr.front().data();
}

float * DiskMesh::GetTangentArr() {
	return tangentArr.front().data();
}

unsigned DiskMesh::GetNormalArrSize() {
	return static_cast<unsigned>(normalArr.size() * 3 * sizeof(float));
}

unsigned DiskMesh::GetTexCoordsArrSize() {
	return static_cast<unsigned>(texCoordsArr.size() * 2 * sizeof(float));
}

unsigned DiskMesh::GetIndexArrSize() {
	return static_cast<unsigned>(indexArr.size() * 3 * sizeof(unsigned));
}

unsigned DiskMesh::GetTangentArrSize() {
	return static_cast<unsigned>(tangentArr.size() * 3 * sizeof(float));
}
