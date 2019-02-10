#ifndef _BASIC_NODE_ELEMENT_H_
#define _BASIC_NODE_ELEMENT_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/EleVisitor.h>

#define ELE_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(CppUtil::Basic::EleVisitor::Ptr eleVisitor){\
	eleVisitor->Visit(This());\
}

#define ELE_SETUP_SELF_DEL(CLASS) \
HEAP_OBJ_SETUP_SELF_DEL(CLASS)\
public:\
virtual void Accept(CppUtil::Basic::EleVisitor::Ptr eleVisitor){\
	eleVisitor->Visit(This());\
}

namespace CppUtil {
	namespace Basic {
		class Element : public HeapObj {
			ELE_SETUP(Element)
		};
	}
}

#endif // !_BASIC_NODE_ELEMENT_H_
