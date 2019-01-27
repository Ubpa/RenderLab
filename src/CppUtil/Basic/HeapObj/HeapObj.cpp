#include <CppUtil/Basic/HeapObj.h>

using namespace CppUtil::Basic;

void HeapObj::ProtectedDelete(const HeapObj * op) {
	delete op;
}