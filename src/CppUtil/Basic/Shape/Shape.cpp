#include <CppUtil/Basic/Shape.h>

using namespace CppUtil::Basic;
using namespace std;

Shape::Shape(uint vertexNum, uint triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = vector<array<float, 3>>(vertexNum);
}

float * Shape::GetPosArr() {
	return posArr.front().data();
}

uint Shape::GetPosArrSize() {
	return static_cast<uint>(posArr.size() * 3 * sizeof(float));
}
