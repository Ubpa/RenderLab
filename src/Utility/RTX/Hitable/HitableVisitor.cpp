#include <Utility/RTX/HitableVisitor.h>

using namespace CppUtility;
using namespace RayTracing;

void HitableVisitor::Visit(Other::CPtr<Hitable> hitable) { }
void HitableVisitor::Visit(Other::CPtr<Sphere> sphere) { }
void HitableVisitor::Visit(Other::CPtr<Sky> sky) { }
void HitableVisitor::Visit(Other::CPtr<MoveSphere> moveSphere) { }
void HitableVisitor::Visit(Other::CPtr<Triangle> triangle) { }
void HitableVisitor::Visit(Other::CPtr<Transform> transform) { }
void HitableVisitor::Visit(Other::CPtr<Volume> volume) { }
void HitableVisitor::Visit(Other::CPtr<Group> group) { }
void HitableVisitor::Visit(Other::CPtr<BVH_Node> bvhNode) { }
void HitableVisitor::Visit(Other::CPtr<TriMesh> triMesh) { }
void HitableVisitor::Visit(Other::CPtr<Model> model) { }