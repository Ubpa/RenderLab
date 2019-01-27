#include <CppUtil/Basic/Cube.h>

#include <CppUtil/Basic/Vec.h>
#include <cmath>

using namespace CppUtil::Basic;

const float Cube::cubeData[192] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,

	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
};

Cube::Cube()
	: Shape(24, 12) {
	normalArr = new Array2D<float>(vertexNum, 3);
	texCoordsArr = new Array2D<float>(vertexNum, 2);
	indexArr = new Array2D<size_t>(triNum, 3);
	//----------
	for (size_t i = 0; i < vertexNum; i++) {
		vertexArr->Copy(i, 0, 3, &(cubeData[8 * i]));
		normalArr->Copy(i, 0, 3, &(cubeData[8 * i + 3]));
		texCoordsArr->Copy(i, 0, 2, &(cubeData[8 * i + 6]));
	}

	const size_t squareIdx[6] = {
		0, 1, 2,
		3, 2, 1
	};

	for (size_t i = 0; i < 6; i++) {
		for (size_t j = 0; j < 6; j++)
			indexArr->At(2 * i + j / 3, j % 3) = 4 * i + squareIdx[j];
	}
}

Cube::~Cube() {
	delete normalArr;
	normalArr = nullptr;
	delete texCoordsArr;
	texCoordsArr = nullptr;
	delete indexArr;
	indexArr = nullptr;
}

float * Cube::GetNormalArr() {
	if (normalArr == nullptr)
		return nullptr;

	return normalArr->GetData();
}

float * Cube::GetTexCoordsArr() {
	if (texCoordsArr == nullptr)
		return nullptr;

	return texCoordsArr->GetData();
}

size_t * Cube::GetIndexArr() {
	if (indexArr == nullptr)
		return nullptr;

	return indexArr->GetData();
}

size_t Cube::GetNormalArrSize() {
	return normalArr->GetMemSize();
}

size_t Cube::GetTexCoordsArrSize() {
	return texCoordsArr->GetMemSize();
}

size_t Cube::GetIndexArrSize() {
	return indexArr->GetMemSize();
}