#include <CppUtil/Basic/Plane.h>

using namespace CppUtil::Basic;
using namespace std;

Plane::Plane()
	: Shape(4, 2) {
	const float vertexData[32] = {
		// positions          // normals           // texture coords
		-0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //left front
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //right front
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //left back
		 0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //right back
	};

	normalArr = vector<array<float, 3>>(vertexNum);
	texCoordsArr = vector<array<float, 2>>(vertexNum);
	indexArr = vector<array<uint, 3>>(triNum);
	
	for (uint i = 0; i < vertexNum; i++) {
		posArr[i] = { vertexData[8 * i], vertexData[8 * i + 1],vertexData[8 * i + 2] };
		normalArr[i] = { vertexData[8 * i + 3], vertexData[8 * i + 4],vertexData[8 * i + 5] };
		texCoordsArr[i] = { vertexData[8 * i + 6], vertexData[8 * i + 7] };
	}

	const uint indice[6] = {
		0, 1, 2,
		3, 2, 1
	};

	indexArr[0] = { indice[0],indice[1],indice[2] };
	indexArr[1] = { indice[3],indice[4],indice[5] };
}

float * Plane::GetNormalArr() {
	return normalArr.front().data();
}

float * Plane::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

uint * Plane::GetIndexArr() {
	return indexArr.front().data();
}

uint Plane::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * 3 * sizeof(float));
}

uint Plane::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * 2 * sizeof(float));
}

uint Plane::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}
