#include <Basic/ShapeMesh/CubeMesh.h>

#include <cmath>

#include <array>

using namespace Ubpa;

using namespace std;

const float CubeMesh::CubeMeshData[192] = {
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

CubeMesh::CubeMesh()
	: ShapeMesh(24, 12) {
	normalArr = vector<array<float, 3>>(vertexNum);
	texCoordsArr = vector<array<float, 2>>(vertexNum);
	indexArr = vector<array<unsigned, 3>>(triNum);
	//----------
	for (unsigned i = 0; i < vertexNum; i++) {
		posArr[i] = { CubeMeshData[8 * i], CubeMeshData[8 * i + 1], CubeMeshData[8 * i + 2] };
		normalArr[i] = { CubeMeshData[8 * i + 3], CubeMeshData[8 * i + 4], CubeMeshData[8 * i + 5] };
		texCoordsArr[i] = { CubeMeshData[8 * i + 6], CubeMeshData[8 * i + 7] };
	}

	const unsigned squareIdx[6] = {
		0, 1, 2,
		3, 2, 1
	};

	for (unsigned i = 0; i < 6; i++) {
		for (unsigned j = 0; j < 6; j++)
			indexArr[2 * i + j / 3][j % 3] = 4 * i + squareIdx[j];
	}
}

float * CubeMesh::GetNormalArr() {
	return normalArr.front().data();
}

float * CubeMesh::GetTexCoordsArr() {
	return texCoordsArr.front().data();
}

unsigned * CubeMesh::GetIndexArr() {
	return indexArr.front().data();
}

unsigned CubeMesh::GetNormalArrSize() {
	return static_cast<unsigned>(normalArr.size() * 3 * sizeof(float));
}

unsigned CubeMesh::GetTexCoordsArrSize() {
	return static_cast<unsigned>(texCoordsArr.size() * 2 * sizeof(float));
}

unsigned CubeMesh::GetIndexArrSize() {
	return static_cast<unsigned>(indexArr.size() * 3 * sizeof(unsigned));
}
