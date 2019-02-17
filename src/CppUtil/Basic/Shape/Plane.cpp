#include <CppUtil/Basic/Plane.h>

using namespace CppUtil::Basic;

Plane::Plane()
	: Shape(4, 2) {
	const float vertexData[32] = {
		// positions          // normals           // texture coords
		-0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //left front
		 0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, //right front
		-0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, //left back
		 0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f, //right back
	};

	normalArr = new Array2D<float>(vertexNum, 3);
	texCoordsArr = new Array2D<float>(vertexNum, 2);
	indexArr = new Array2D<uint>(triNum, 3);
	
	for (uint i = 0; i < vertexNum; i++) {
		posArr->Copy(i, 0, 3, &(vertexData[8 * i]));
		normalArr->Copy(i, 0, 3, &(vertexData[8 * i + 3]));
		texCoordsArr->Copy(i, 0, 2, &(vertexData[8 * i + 6]));
	}

	const uint indice[6] = {
		0, 1, 2,
		3, 2, 1
	};

	indexArr->Copy(0, 0, 6, indice);
}


Plane::~Plane() {
	delete normalArr;
	normalArr = nullptr;
	delete texCoordsArr;
	texCoordsArr = nullptr;
	delete indexArr;
	indexArr = nullptr;
}

float * Plane::GetNormalArr() {
	if (normalArr == nullptr)
		return nullptr;

	return normalArr->GetData();
}

float * Plane::GetTexCoordsArr() {
	if (texCoordsArr == nullptr)
		return nullptr;

	return texCoordsArr->GetData();
}

uint * Plane::GetIndexArr() {
	if (indexArr == nullptr)
		return nullptr;

	return indexArr->GetData();
}

uint Plane::GetNormalArrSize() {
	return normalArr->GetMemSize();
}

uint Plane::GetTexCoordsArrSize() {
	return texCoordsArr->GetMemSize();
}

uint Plane::GetIndexArrSize() {
	return indexArr->GetMemSize();
}
