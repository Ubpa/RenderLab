#include <Basic/Disk.h>

#include <Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Disk::Disk(uint n)
	: Shape(n+1, n)
{
	normalArr = vector<Vec3f>(n + 1);
	texCoordsArr = vector<Vec2f>(n + 1);
	tangentArr = vector<Vec3f>(n + 1);
	indexArr = vector<uVec3i>(n);

	float deltaTheta = 2 * Math::PI / n;
	float y = 0;
	for (uint i = 0; i < n; i++) {
		float theta = i * deltaTheta;
		float z = cosf(theta);
		float x = sinf(theta);
		posArr[i] = Vec3f(x, y, z);
		texCoordsArr[i] = Vec2f((1 + x) / 2, (1 + z) / 2);
	}
	posArr[n] = Vec3f(0,0,0);
	texCoordsArr[n] = Vec2f(0.5, 0.5);

	for (uint i = 0; i < n + 1; i++) {
		normalArr[i] = Vec3f(0,1,0);
		tangentArr[i] = Vec3(1,0,0);
	}

	for (uint i = 0; i < n; i++) {
		int next = (i + 1) % n;
		indexArr[i] = uVec3i(i,next,n);
	}
}

float * Disk::GetNormalArr() {
	return normalArr.front().Data();
}

float * Disk::GetTexCoordsArr() {
	return texCoordsArr.front().Data();
}

uint * Disk::GetIndexArr() {
	return indexArr.front().Data();
}

float * Disk::GetTangentArr() {
	return tangentArr.front().Data();
}

uint Disk::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * 3 * sizeof(float));
}

uint Disk::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * 2 * sizeof(float));
}

uint Disk::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}

uint Disk::GetTangentArrSize() {
	return static_cast<uint>(tangentArr.size() * 3 * sizeof(float));
}
