#include <CppUtil/Basic/Sphere.h>

#include <CppUtil/Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Sphere::Sphere(uint n)
	: Shape((n + 1)*(n + 1), 2 * n*n) {
	// normal 和 pos 的值相同
	for (auto & pos : posArr)
		normalArr.push_back(pos);
	texCoordsArr = vector<Point2>(vertexNum);
	indexArr = vector<Val<3,uint>>(triNum);
	tangentArr = vector<Normalf>(vertexNum);

	//----------
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
			posArr[i*(n + 1) + j].x = x;
			posArr[i*(n + 1) + j].y = y;
			posArr[i*(n + 1) + j].z = z;
			
			texCoordsArr[i*(n + 1) + j].x = u;
			texCoordsArr[i*(n + 1) + j].y = v;

			tangentArr[i*(n + 1) + j].x = cos(phi);
			tangentArr[i*(n + 1) + j].y = 0;
			tangentArr[i*(n + 1) + j].z = -sin(phi);
		}
	}
	//------------
	for (uint i = 0; i < n; i++) {
		for (uint j = 0; j < n; j++) {
			indexArr[2 * (i*n + j)].x = (i + 1) * (n + 1) + j;
			indexArr[2 * (i*n + j)].y = i * (n + 1) + j;
			indexArr[2 * (i*n + j)].z = i * (n + 1) + j + 1;

			indexArr[2 * (i*n + j) + 1].x = i * (n + 1) + j + 1;
			indexArr[2 * (i*n + j) + 1].y = (i + 1) * (n + 1) + j + 1;
			indexArr[2 * (i*n + j) + 1].z = (i + 1) * (n + 1) + j;
		}
	}
}

float * Sphere::GetNormalArr() {
	if (normalArr.empty())
		return nullptr;

	return normalArr.front().Data();
}

float * Sphere::GetTexCoordsArr() {
	if (texCoordsArr.empty())
		return nullptr;

	return texCoordsArr.front().Data();
}

uint * Sphere::GetIndexArr() {
	if (indexArr.empty())
		return nullptr;

	return indexArr.front().Data();
}

float * Sphere::GetTangentArr() {
	if (tangentArr.empty())
		return nullptr;

	return tangentArr.front().Data();
}

uint Sphere::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * sizeof(Normalf));
}

uint Sphere::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * sizeof(Point2));
}

uint Sphere::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}

uint Sphere::GetTangentArrSize() {
	return static_cast<uint>(tangentArr.size() * sizeof(Normalf));
}
