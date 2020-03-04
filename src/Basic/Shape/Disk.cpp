#include <Basic/Disk.h>

#include <Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Disk::Disk(unsigned n)
	: Shape(n+1, n)
{
	normalArr = vector<Ubpa::vecf3>(n + 1);
	texCoordsArr = vector<Ubpa::vecf2>(n + 1);
	tangentArr = vector<Ubpa::vecf3>(n + 1);
	indexArr = vector<Ubpa::valu3>(n);

	float deltaTheta = 2 * Math::PI / n;
	float y = 0;
	for (unsigned i = 0; i < n; i++) {
		float theta = i * deltaTheta;
		float z = cosf(theta);
		float x = sinf(theta);
		posArr[i] = Ubpa::vecf3(x, y, z);
		texCoordsArr[i] = Ubpa::vecf2((1 + x) / 2, (1 + z) / 2);
	}
	posArr[n] = Ubpa::vecf3(0,0,0);
	texCoordsArr[n] = Ubpa::vecf2(0.5, 0.5);

	for (unsigned i = 0; i < n + 1; i++) {
		normalArr[i] = Ubpa::vecf3(0,1,0);
		tangentArr[i] = Ubpa::vecf3(1,0,0);
	}

	for (unsigned i = 0; i < n; i++) {
		int next = (i + 1) % n;
		indexArr[i] = Ubpa::valu3(i,next,n);
	}
}

float * Disk::GetNormalArr() {
	return normalArr.front().data();
}

float * Disk::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

unsigned * Disk::GetIndexArr() {
	return indexArr.front().data();
}

float * Disk::GetTangentArr() {
	return tangentArr.front().data();
}

unsigned Disk::GetNormalArrSize() {
	return static_cast<unsigned>(normalArr.size() * 3 * sizeof(float));
}

unsigned Disk::GetTexCoordsArrSize() {
	return static_cast<unsigned>(texCoordsArr.size() * 2 * sizeof(float));
}

unsigned Disk::GetIndexArrSize() {
	return static_cast<unsigned>(indexArr.size() * 3 * sizeof(unsigned));
}

unsigned Disk::GetTangentArrSize() {
	return static_cast<unsigned>(tangentArr.size() * 3 * sizeof(float));
}
