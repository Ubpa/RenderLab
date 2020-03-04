#include <Basic/Shape.h>

using namespace CppUtil::Basic;
using namespace std;

Shape::Shape(unsigned vertexNum, unsigned triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = vector<Ubpa::vecf3>(vertexNum);
}

float * Shape::GetPosArr() {
	return posArr.front().data();
}

unsigned Shape::GetPosArrSize() {
	return static_cast<unsigned>(posArr.size() * 3 * sizeof(float));
}
