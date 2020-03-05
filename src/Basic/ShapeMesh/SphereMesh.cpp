#include <Basic/ShapeMesh/SphereMesh.h>

#include <Basic/Math.h>

#include <cmath>

using namespace Ubpa;

using namespace std;

SphereMesh::SphereMesh(unsigned n)
	: ShapeMesh((n + 1)*(n + 1), 2 * n*n) {
	texCoordsArr = vector<vecf2>(vertexNum);
	indexArr = vector<valu3>(triNum);
	tangentArr = vector<vecf3>(vertexNum);

	float inc = 1.0f / n;
	for (unsigned i = 0; i <= n; i++) {
		float u = inc * i;
		for (unsigned j = 0; j <= n; j++) {
			float v = inc * j;
			float phi = 2 * Math::PI * u;
			float theta = Math::PI * v;

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);

			// 右手系: 上y, 右x, 垂直屏幕外为 z
			posArr[i*(n + 1) + j][0] = x;
			posArr[i*(n + 1) + j][1] = y;
			posArr[i*(n + 1) + j][2] = z;
			
			texCoordsArr[i*(n + 1) + j][0] = u;
			texCoordsArr[i*(n + 1) + j][1] = v;

			tangentArr[i*(n + 1) + j][0] = cos(phi);
			tangentArr[i*(n + 1) + j][1] = 0;
			tangentArr[i*(n + 1) + j][2] = -sin(phi);
		}
	}
	
	for (unsigned i = 0; i < n; i++) {
		for (unsigned j = 0; j < n; j++) {
			indexArr[2 * (i*n + j)][0] = (i + 1) * (n + 1) + j;
			indexArr[2 * (i*n + j)][1] = i * (n + 1) + j;
			indexArr[2 * (i*n + j)][2] = i * (n + 1) + j + 1;

			indexArr[2 * (i*n + j) + 1][0] = i * (n + 1) + j + 1;
			indexArr[2 * (i*n + j) + 1][1] = (i + 1) * (n + 1) + j + 1;
			indexArr[2 * (i*n + j) + 1][2] = (i + 1) * (n + 1) + j;
		}
	}

	// normal 和 pos 的值相同
	for (auto & pos : posArr)
		normalArr.push_back(pos);
}

float * SphereMesh::GetNormalArr() {
	return normalArr.front().data();
}

float * SphereMesh::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

unsigned * SphereMesh::GetIndexArr() {
	return indexArr.front().data();
}

float * SphereMesh::GetTangentArr() {
	return tangentArr.front().data();
}

unsigned SphereMesh::GetNormalArrSize() {
	return static_cast<unsigned>(normalArr.size() * 3 * sizeof(float));
}

unsigned SphereMesh::GetTexCoordsArrSize() {
	return static_cast<unsigned>(texCoordsArr.size() * 2 * sizeof(float));
}

unsigned SphereMesh::GetIndexArrSize() {
	return static_cast<unsigned>(indexArr.size() * 3 * sizeof(unsigned));
}

unsigned SphereMesh::GetTangentArrSize() {
	return static_cast<unsigned>(tangentArr.size() * 3 * sizeof(float));
}
