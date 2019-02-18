#include <CppUtil/RTX/MatVisitor.h>

using namespace RTX;
using namespace CppUtil;

void MatVisitor::Visit(Basic::CPtr<Material> material) { }
void MatVisitor::Visit(Basic::CPtr<Lambertian> lambertian) { }
void MatVisitor::Visit(Basic::CPtr<Metal> metal) { }
void MatVisitor::Visit(Basic::CPtr<Dielectric> dielectric) { }
void MatVisitor::Visit(Basic::CPtr<Light> light) { }
void MatVisitor::Visit(Basic::CPtr<OpMaterial> opMaterial) { }
void MatVisitor::Visit(Basic::CPtr<Isotropic> isotropic) { }
