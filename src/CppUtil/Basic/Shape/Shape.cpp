#include <CppUtil/Basic/Shape.h>

using namespace CppUtil::Basic;

const float Shape::PI = 3.1415926;

Shape::Shape(size_t vertexNum, size_t triNum)
	: triNum(triNum), vertexNum(vertexNum){
	vertexArr = new Array2D<float>(vertexNum, 3);
}

size_t Shape::GetTriNum() {
	return triNum;
}

Shape::~Shape() {
	delete vertexArr;
	vertexArr = nullptr;
}

float * Shape::GetVertexArr() {
	if (vertexArr == nullptr)
		return nullptr;
	
	return vertexArr->GetData();
}

size_t Shape::GetVertexArrSize() {
	return vertexArr->GetMemSize();
}
