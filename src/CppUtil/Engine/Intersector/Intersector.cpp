#include <CppUtil/Engine/BVHNode.h>

#include <CppUtil/Engine/Triangle.h>

using namespace CppUtil::Engine;

template <>
const BBox BVHNode<Triangle>::GetBBox(Triangle::Ptr obj) {
	return obj->GetBBox();
}
