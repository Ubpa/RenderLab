#ifndef _HITABLE_VISITOR_H_
#define _HITABLE_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>

namespace RTX {
	class Hitable;
	class Sphere;
	class Sky;
	class MoveSphere;
	class Triangle;
	class Transform;
	class Volume;
	class Group;
	class BVH_Node;
	class TriMesh;
	class Model;

	class HitableVisitor : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(HitableVisitor)
	public:
		virtual void Visit(CppUtil::Basic::CPtr<Hitable> sphere);
		virtual void Visit(CppUtil::Basic::CPtr<Sphere> sphere);
		virtual void Visit(CppUtil::Basic::CPtr<Sky> sky);
		virtual void Visit(CppUtil::Basic::CPtr<MoveSphere> moveSphere);
		virtual void Visit(CppUtil::Basic::CPtr<Triangle> triangle);
		virtual void Visit(CppUtil::Basic::CPtr<Transform> transform);
		virtual void Visit(CppUtil::Basic::CPtr<Volume> volume);
		virtual void Visit(CppUtil::Basic::CPtr<Group> group);
		virtual void Visit(CppUtil::Basic::CPtr<BVH_Node> bvhNode);
		virtual void Visit(CppUtil::Basic::CPtr<TriMesh> triMesh);
		virtual void Visit(CppUtil::Basic::CPtr<Model> model);
	};
}

#endif // !_HITABLE_VISITOR_H_
