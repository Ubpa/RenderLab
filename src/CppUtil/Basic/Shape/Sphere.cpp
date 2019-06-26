#include <CppUtil/Basic/Sphere.h>

#include <CppUtil/Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Sphere::Sphere(uint n)
	: Shape((n + 1)*(n + 1), 2 * n*n) {
	texCoordsArr = vector<Vec2f>(vertexNum);
	indexArr = vector<uVec3i>(triNum);
	tangentArr = vector<Vec3f>(vertexNum);

	float inc = 1.0f / n;
	for (uint i = 0; i <= n; i++) {
		float u = inc * i;
		for (uint j = 0; j <= n; j++) {
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
	
	for (uint i = 0; i < n; i++) {
		for (uint j = 0; j < n; j++) {
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

float * Sphere::GetNormalArr() {
	return normalArr.front().Data();
}

float * Sphere::GetTexCoordsArr() {
	return texCoordsArr.front().Data();
}

uint * Sphere::GetIndexArr() {
	return indexArr.front().Data();
}

float * Sphere::GetTangentArr() {
	return tangentArr.front().Data();
}

uint Sphere::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * 3 * sizeof(float));
}

uint Sphere::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * 2 * sizeof(float));
}

uint Sphere::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}

uint Sphere::GetTangentArrSize() {
	return static_cast<uint>(tangentArr.size() * 3 * sizeof(float));
}
