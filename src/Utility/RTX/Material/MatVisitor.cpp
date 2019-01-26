#include <Utility/RTX/MatVisitor.h>

using namespace RayTracing;
using namespace CppUtility;

void MatVisitor::Visit(Other::CPtr<Material> material) { }
void MatVisitor::Visit(Other::CPtr<Lambertian> lambertian) { }
void MatVisitor::Visit(Other::CPtr<Metal> metal) { }
void MatVisitor::Visit(Other::CPtr<Dielectric> dielectric) { }
void MatVisitor::Visit(Other::CPtr<Light> light) { }
void MatVisitor::Visit(Other::CPtr<OpMaterial> opMaterial) { }
void MatVisitor::Visit(Other::CPtr<Isotropic> isotropic) { }