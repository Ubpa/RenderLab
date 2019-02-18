#include <CppUtil/RTX/HitableVisitor.h>

using namespace CppUtil;
using namespace RTX;

void HitableVisitor::Visit(Basic::CPtr<Hitable> hitable) { }
void HitableVisitor::Visit(Basic::CPtr<Sphere> sphere) { }
void HitableVisitor::Visit(Basic::CPtr<Sky> sky) { }
void HitableVisitor::Visit(Basic::CPtr<MoveSphere> moveSphere) { }
void HitableVisitor::Visit(Basic::CPtr<Triangle> triangle) { }
void HitableVisitor::Visit(Basic::CPtr<Transform> transform) { }
void HitableVisitor::Visit(Basic::CPtr<Volume> volume) { }
void HitableVisitor::Visit(Basic::CPtr<Group> group) { }
void HitableVisitor::Visit(Basic::CPtr<BVH_Node> bvhNode) { }
void HitableVisitor::Visit(Basic::CPtr<TriMesh> triMesh) { }
void HitableVisitor::Visit(Basic::CPtr<Model> model) { }
