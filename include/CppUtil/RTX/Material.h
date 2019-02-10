#ifndef _RTX_RT_MATERIAL_MATERIAL_H_
#define _RTX_RT_MATERIAL_MATERIAL_H_

#include <CppUtil/RTX/MatVisitor.h>
#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/RTX/HitRecord.h>

#define MATERIAL_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(MatVisitor::Ptr matVisitor) const{\
	matVisitor->Visit(CThis());\
}

namespace RTX {
	class Ray;

	class Material : public CppUtil::Basic::HeapObj{
		HEAP_OBJ_SETUP(Material)
	public:
		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const = 0;
		virtual void Accept(MatVisitor::Ptr matVisitor) const = 0;
	};
}

#endif // !_RTX_RT_MATERIAL_MATERIAL_H_
