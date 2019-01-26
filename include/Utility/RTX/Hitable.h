#ifndef _HITABLE_H_
#define _HITABLE_H_ 

#include <Utility/RTX/AABB.h>
#include <Utility/RTX/Material.h>

#include <Utility/RTX/HitableVisitor.h>

#define HITABLE_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(HitableVisitor::Ptr hitableVisitor) const{\
	hitableVisitor->Visit(CThis());\
}


namespace RayTracing {
	class Ray;

	struct HitRst {
		HitRst(bool hit = false) : hit(hit), isMatCoverable(true), material(NULL) { }

		bool hit;
		HitRecord record;
		CppUtility::Other::CPtr<Material> material;
		bool isMatCoverable;
		//------------
		static const HitRst InValid;
	};

	class Hitable : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Hitable)
	public:
		Hitable(Material::CPtr material = NULL);

		const Material::CPtr GetMat() const { return material; };
		bool IsMatCoverable() const { return isMatCoverable; }

		virtual HitRst RayIn(CppUtility::Other::Ptr<Ray> & ray) const = 0;
		virtual const AABB GetBoundingBox() const = 0;
		virtual void Accept(HitableVisitor::Ptr hitableVisitor) const = 0;
	private:
		Material::CPtr material;
		bool isMatCoverable;
	};
}
#endif