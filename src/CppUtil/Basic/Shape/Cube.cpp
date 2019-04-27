#include <CppUtil/Basic/Cube.h>

#include <cmath>

using namespace CppUtil::Basic;
using namespace std;

const float Cube::cubeData[192] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,

	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
};

Cube::Cube()
	: Shape(24, 12) {
	normalArr = vector<Normalf>(vertexNum);
	texCoordsArr = vector<Point2>(vertexNum);
	indexArr = vector<Val<3, uint>>(triNum);
	//----------
	for (uint i = 0; i < vertexNum; i++) {
		posArr[i] = { cubeData[8 * i], cubeData[8 * i + 1], cubeData[8 * i + 2] };
		normalArr[i] = { cubeData[8 * i + 3], cubeData[8 * i + 4], cubeData[8 * i + 5] };
		texCoordsArr[i] = { cubeData[8 * i + 6], cubeData[8 * i + 7] };
	}

	const uint squareIdx[6] = {
		0, 1, 2,
		3, 2, 1
	};

	for (uint i = 0; i < 6; i++) {
		for (uint j = 0; j < 6; j++)
			indexArr[2 * i + j / 3][j % 3] = 4 * i + squareIdx[j];
	}
}

float * Cube::GetNormalArr() {
	if (normalArr.empty())
		return nullptr;

	return normalArr.front().Data();
}

float * Cube::GetTexCoordsArr() {
	if (texCoordsArr.empty())
		return nullptr;

	return texCoordsArr.front().Data();
}

uint * Cube::GetIndexArr() {
	if (indexArr.empty())
		return nullptr;

	return indexArr.front().Data();
}

uint Cube::GetNormalArrSize() {
	return static_cast<uint>(normalArr.size() * sizeof(Normalf));
}

uint Cube::GetTexCoordsArrSize() {
	return static_cast<uint>(texCoordsArr.size() * sizeof(Point2));
}

uint Cube::GetIndexArrSize() {
	return static_cast<uint>(indexArr.size() * 3 * sizeof(uint));
}
