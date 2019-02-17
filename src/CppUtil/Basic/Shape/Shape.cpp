#include <CppUtil/Basic/Shape.h>

using namespace CppUtil::Basic;

const float Shape::PI = 3.1415926f;

Shape::Shape(uint vertexNum, uint triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = new Array2D<float>(vertexNum, 3);
}

Shape::~Shape() {
	delete posArr;
	posArr = nullptr;
}

float * Shape::GetPosArr() {
	if (posArr == nullptr)
		return nullptr;
	
	return posArr->GetData();
}

uint Shape::GetPosArrSize() {
	return posArr->GetMemSize();
}
