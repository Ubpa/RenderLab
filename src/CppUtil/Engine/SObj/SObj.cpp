#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Component.h>

using namespace CppUtil::Engine;

SObj::~SObj() {
	// 注意避免循环引用

	// 因为 Detach() 会删除 component，所以应该先取到下一个 component
	auto next = components.begin();
	while (next != components.end()) {
		auto cur = next;
		next++;
		(*cur)->Detach();
	}
	
}