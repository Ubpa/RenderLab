#ifndef _MAT_VISITOR_H_
#define _MAT_VISITOR_H_

#include <Utility/Basic/HeapObj.h>

namespace RayTracing {
	class Material;
	class Lambertian;
	class Metal;
	class Dielectric;
	class Light;
	class OpMaterial;
	class Isotropic;

	class MatVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(MatVisitor)
	public:
		virtual void Visit(CppUtility::Other::CPtr<Material> material);
		virtual void Visit(CppUtility::Other::CPtr<Lambertian> lambertian);
		virtual void Visit(CppUtility::Other::CPtr<Metal> metal);
		virtual void Visit(CppUtility::Other::CPtr<Dielectric> dielectric);
		virtual void Visit(CppUtility::Other::CPtr<Light> light);
		virtual void Visit(CppUtility::Other::CPtr<OpMaterial> opMaterial);
		virtual void Visit(CppUtility::Other::CPtr<Isotropic> isotropic);
	};
}

#endif // !_MAT_VISITOR_H_
