#include <CppUtil/Basic/Shape.h>

using namespace CppUtil::Basic;
using namespace std;

Shape::Shape(uint vertexNum, uint triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = vector<Point3>(vertexNum);
}

float * Shape::GetPosArr() {
	if (posArr.empty())
		return nullptr;
	
	return posArr.front().Data();
}

uint Shape::GetPosArrSize() {
	return static_cast<uint>(posArr.size() * sizeof(Point3));
}
