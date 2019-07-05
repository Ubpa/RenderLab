#include <CppUtil/Basic/Capsule.h>

#include <CppUtil/Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

Capsule::Capsule(uint n, float height)
	: Shape((2*n+1)*(2 * n +2), 4 * n * (2 * n +1)), height(height)
{
	normalArr = vector<Vec3f>(vertexNum);
	texCoordsArr = vector<Vec2f>(vertexNum);
	indexArr = vector<uVec3i>(triNum);

	float halfH = height / 2;

	// …œ∞Î«Ú
	for (uint i = 0; i <= 2 * n; i++) {
		for (uint j = 0; j <= n; j++) {
			float phi = 2 * Math::PI * i / float(2 * n);
			float theta = Math::PI * j / float(2 * n);

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);
			posArr[i + j * (2 * n + 1)] = Vec3f(x, y + halfH, z);
			normalArr[i + j * (2 * n + 1)] = Vec3f(x, y, z);
		}
	}

	// œ¬∞Î«Ú
	for (uint i = 0; i <= 2 * n; i++) {
		for (uint j = n + 1; j <= 2 * n + 1; j++) {
			float phi = 2 * Math::PI * i / float(2 * n);
			float theta = Math::PI * (j-1) / float(2 * n);

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);
			posArr[i + j * (2 * n + 1)] = Vec3f(x, y - halfH, z);
			normalArr[i + j * (2 * n + 1)] = Vec3f(x, y, z);
		}
	}

	for (uint i = 0; i < vertexNum; i++) {
		auto norm = posArr[i].Normalize();

		float phi = std::atan2(-norm.x, -norm.z) + Math::PI;
		float theta = acos(norm.y);

		auto u = phi / (2.f * Math::PI);
		auto v = theta / Math::PI;
		
		texCoordsArr[i] = Vec2f(u, v);
	}

	for (uint i = 0; i < 2*n; i++) {
		for (uint j = 0; j < 2*n+1; j++) {
			// 1 2
			// 3 4

			// 2 1 4
			indexArr[2 * (i + j * (2 * n + 1))][0] = (i + 1) + (j) * (2 * n + 1); // 2
			indexArr[2 * (i + j * (2 * n + 1))][1] = (i) + (j) * (2 * n + 1); // 1
			indexArr[2 * (i + j * (2 * n + 1))][2] = (i) + (j + 1) * (2 * n + 1); // 4

			// 3 4 1
			indexArr[2 * (i + j * (2 * n + 1)) + 1][0] = (i) + (j + 1) * (2 * n + 1); // 3
			indexArr[2 * (i + j * (2 * n + 1)) + 1][1] = (i + 1) + (j + 1) * (2 * n + 1); // 4
			indexArr[2 * (i + j * (2 * n + 1)) + 1][2] = (i) + (j) * (2 * n + 1); // 1
		}
	}
}

float * Capsule::GetNormalArr() {
	return normalArr.front().Data();
}

float * Capsule::GetTexCoordsArr() {
	return texCoordsArr.front().Data();
}

uint * Capsule::GetIndexArr() {
	return indexArr.front().Data();
}

uint Capsule::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * 3 * sizeof(float));
}

uint Capsule::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * 2 * sizeof(float));
}

uint Capsule::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}
