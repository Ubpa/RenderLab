#include <Basic/ShapeMesh/ShapeMesh.h>

using namespace Ubpa;

using namespace std;

ShapeMesh::ShapeMesh(unsigned vertexNum, unsigned triNum)
	: vertexNum(vertexNum), triNum(triNum){
	posArr = vector<vecf3>(vertexNum);
}

float * ShapeMesh::GetPosArr() {
	return posArr.front().data();
}

unsigned ShapeMesh::GetPosArrSize() {
	return static_cast<unsigned>(posArr.size() * 3 * sizeof(float));
}
