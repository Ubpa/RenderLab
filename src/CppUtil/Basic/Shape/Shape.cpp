#include <CppUtil/Basic/Shape.h>

using namespace CppUtil::Basic;
using namespace std;

Shape::Shape(uint vertexNum, uint triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = vector<Vec3f>(vertexNum);
}

float * Shape::GetPosArr() {
	return posArr.front().Data();
}

uint Shape::GetPosArrSize() {
	return static_cast<uint>(posArr.size() * 3 * sizeof(float));
}
