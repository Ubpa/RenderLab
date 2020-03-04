#include <Basic/Capsule.h>

#include <Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Capsule::Capsule(unsigned n, float height)
	: Shape((2*n+1)*(2 * n +2), 4 * n * (2 * n +1)), height(height)
{
	// 纵向 2n+1 格，2n+2 个点
	// 横向 2n 格，2n+1 个点
	// (2n+2)(2n+1) 个点
	// 2(2n)(2n+1) == 8n^2 + 4n 个三角形

	normalArr = vector<Ubpa::vecf3>(vertexNum);
	texCoordsArr = vector<Ubpa::vecf2>(vertexNum);
	indexArr = vector<Ubpa::valu3>(triNum);
	tangentArr = vector<Ubpa::vecf3>(vertexNum);

	float halfH = height / 2;

	// 上半球
	for (unsigned i = 0; i <= 2 * n; i++) {
		for (unsigned j = 0; j <= n; j++) {
			float phi = 2 * Math::PI * i / float(2 * n);
			float theta = Math::PI * j / float(2 * n);

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);
			posArr[i + j * (2 * n + 1)] = Ubpa::vecf3(x, y + halfH, z);
			normalArr[i + j * (2 * n + 1)] = Ubpa::vecf3(x, y, z);
		}
	}

	// 下半球
	for (unsigned i = 0; i <= 2 * n; i++) {
		for (unsigned j = n + 1; j <= 2 * n + 1; j++) {
			float phi = 2 * Math::PI * i / float(2 * n);
			float theta = Math::PI * (j-1) / float(2 * n);

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);
			posArr[i + j * (2 * n + 1)] = Ubpa::vecf3(x, y - halfH, z);
			normalArr[i + j * (2 * n + 1)] = Ubpa::vecf3(x, y, z);
		}
	}

	for (unsigned i = 0; i < vertexNum; i++) {
		auto norm = posArr[i].normalize();

		float phi = std::atan2(-norm[0], -norm[2]) + Math::PI;
		float theta = acos(norm[1]);

		auto u = phi / (2.f * Math::PI);
		auto v = theta / Math::PI;
		
		texCoordsArr[i] = Ubpa::vecf2(u, v);
		tangentArr[i] = Ubpa::vecf3(cos(phi), 0, -sin(phi));
	}

	for (unsigned i = 0; i < 2 * n; i++) {
		for (unsigned j = 0; j < 2 * n + 1; j++) {
			// 1 2
			// 3 4

			// 2 1 4
			indexArr[2 * (i + (j * 2 * n))][0] = (i + 1) + (j + 0) * (2 * n + 1); // 2
			indexArr[2 * (i + (j * 2 * n))][1] = (i + 0) + (j + 0) * (2 * n + 1); // 1
			indexArr[2 * (i + (j * 2 * n))][2] = (i + 1) + (j + 1) * (2 * n + 1); // 4

			// 3 4 1
			indexArr[2 * (i + (j * 2 * n)) + 1][0] = (i + 0) + (j + 1) * (2 * n + 1); // 3
			indexArr[2 * (i + (j * 2 * n)) + 1][1] = (i + 1) + (j + 1) * (2 * n + 1); // 4
			indexArr[2 * (i + (j * 2 * n)) + 1][2] = (i + 0) + (j + 0) * (2 * n + 1); // 1
		}
	}
}

float * Capsule::GetNormalArr() {
	return normalArr.front().data();
}

float * Capsule::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

unsigned * Capsule::GetIndexArr() {
	return indexArr.front().data();
}

float * Capsule::GetTangentArr() {
	return tangentArr.front().data();
}

unsigned Capsule::GetNormalArrSize() {
	return static_cast<unsigned>(normalArr.size() * 3 * sizeof(float));
}

unsigned Capsule::GetTexCoordsArrSize() {
	return static_cast<unsigned>(texCoordsArr.size() * 2 * sizeof(float));
}

unsigned Capsule::GetIndexArrSize() {
	return static_cast<unsigned>(indexArr.size() * 3 * sizeof(unsigned));
}

unsigned Capsule::GetTangentArrSize() {
	return static_cast<unsigned>(tangentArr.size() * 3 * sizeof(float));
}
