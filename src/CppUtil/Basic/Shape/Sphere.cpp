#include <CppUtil/Basic/Sphere.h>

#include <CppUtil/Basic/Math.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace glm;

Sphere::Sphere(uint n)
	: Shape((n + 1)*(n + 1), 2 * n*n) {
	// normal 和 pos 的值相同
	normalArr = posArr;
	texCoordsArr = new Array2D<float>(vertexNum, 2);
	indexArr = new Array2D<uint>(triNum, 3);
	tangentArr = new Array2D<float>(vertexNum, 3);

	//----------
	float inc = 1.0f / n;
	for (uint i = 0; i <= n; i++) {
		float u = inc * i;
		for (uint j = 0; j <= n; j++) {
			float v = inc * j;
			float phi = 2 * PI * u;
			float theta = PI * v;

			float x = sinf(theta) * sinf(phi);
			float y = cosf(theta);
			float z = sinf(theta) * cosf(phi);

			// 右手系: 上y, 右x, 垂直屏幕外为 z
			posArr->At(i*(n + 1) + j, 0) = x;
			posArr->At(i*(n + 1) + j, 1) = y;
			posArr->At(i*(n + 1) + j, 2) = z;
			
			texCoordsArr->At(i*(n + 1) + j, 0) = u;
			texCoordsArr->At(i*(n + 1) + j, 1) = v;

			tangentArr->At(i*(n + 1) + j, 0) = cos(phi);
			tangentArr->At(i*(n + 1) + j, 1) = 0;
			tangentArr->At(i*(n + 1) + j, 2) = -sin(phi);
		}
	}
	//------------
	for (uint i = 0; i < n; i++) {
		for (uint j = 0; j < n; j++) {
			indexArr->At(2 * (i*n + j), 0) = (i + 1) * (n + 1) + j;
			indexArr->At(2 * (i*n + j), 1) = i * (n + 1) + j;
			indexArr->At(2 * (i*n + j), 2) = i * (n + 1) + j + 1;

			indexArr->At(2 * (i*n + j) + 1, 0) = i * (n + 1) + j + 1;
			indexArr->At(2 * (i*n + j) + 1, 1) = (i + 1) * (n + 1) + j + 1;
			indexArr->At(2 * (i*n + j) + 1, 2) = (i + 1) * (n + 1) + j;
		}
	}
}

Sphere::~Sphere() {
	// normalArr 和 posArr 相同, 故无需释放normalArr
	normalArr = nullptr;
	delete texCoordsArr;
	texCoordsArr = nullptr;
	delete indexArr;
	indexArr = nullptr;
	delete tangentArr;
	tangentArr = nullptr;
}

float * Sphere::GetNormalArr() {
	if (normalArr == nullptr)
		return nullptr;

	return normalArr->GetData();
}

float * Sphere::GetTexCoordsArr() {
	if (texCoordsArr == nullptr)
		return nullptr;

	return texCoordsArr->GetData();
}

uint * Sphere::GetIndexArr() {
	if (indexArr == nullptr)
		return nullptr;

	return indexArr->GetData();
}

uint Sphere::GetNormalArrSize() {
	return normalArr->GetMemSize();
}

uint Sphere::GetTexCoordsArrSize() {
	return texCoordsArr->GetMemSize();
}

uint Sphere::GetIndexArrSize() {
	return indexArr->GetMemSize();
}

float * Sphere::GetTangentArr() {
	if (tangentArr == nullptr)
		return nullptr;

	return tangentArr->GetData();
}

uint Sphere::GetTangentArrSize() {
	return normalArr->GetMemSize();
}
