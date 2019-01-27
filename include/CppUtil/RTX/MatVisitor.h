#ifndef _MAT_VISITOR_H_
#define _MAT_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>

namespace RTX {
	class Material;
	class Lambertian;
	class Metal;
	class Dielectric;
	class Light;
	class OpMaterial;
	class Isotropic;

	class MatVisitor : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(MatVisitor)
	public:
		virtual void Visit(CppUtil::Basic::CPtr<Material> material);
		virtual void Visit(CppUtil::Basic::CPtr<Lambertian> lambertian);
		virtual void Visit(CppUtil::Basic::CPtr<Metal> metal);
		virtual void Visit(CppUtil::Basic::CPtr<Dielectric> dielectric);
		virtual void Visit(CppUtil::Basic::CPtr<Light> light);
		virtual void Visit(CppUtil::Basic::CPtr<OpMaterial> opMaterial);
		virtual void Visit(CppUtil::Basic::CPtr<Isotropic> isotropic);
	};
}

#endif // !_MAT_VISITOR_H_
