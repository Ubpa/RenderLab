#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/EleVisitor.h>

#define ELE_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(CppUtil::Basic::EleVisitor::Ptr eleVisitor){\
	eleVisitor->Visit(This());\
	eleVisitor->Visit(CThis());\
}\
virtual void Accept(CppUtil::Basic::EleVisitor::Ptr eleVisitor) const{\
	eleVisitor->Visit(CThis());\
}

namespace CppUtil {
	namespace Basic {
		class Element : public HeapObj {
			ELE_SETUP(Element)
		};
	}
}

#endif // !_ELEMENT_H_
