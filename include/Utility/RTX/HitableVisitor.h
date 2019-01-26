#ifndef _HITABLE_VISITOR_H_
#define _HITABLE_VISITOR_H_

#include <Utility/Basic/HeapObj.h>

namespace RayTracing {
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

	class HitableVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(HitableVisitor)
	public:
		virtual void Visit(CppUtility::Other::CPtr<Hitable> sphere);
		virtual void Visit(CppUtility::Other::CPtr<Sphere> sphere);
		virtual void Visit(CppUtility::Other::CPtr<Sky> sky);
		virtual void Visit(CppUtility::Other::CPtr<MoveSphere> moveSphere);
		virtual void Visit(CppUtility::Other::CPtr<Triangle> triangle);
		virtual void Visit(CppUtility::Other::CPtr<Transform> transform);
		virtual void Visit(CppUtility::Other::CPtr<Volume> volume);
		virtual void Visit(CppUtility::Other::CPtr<Group> group);
		virtual void Visit(CppUtility::Other::CPtr<BVH_Node> bvhNode);
		virtual void Visit(CppUtility::Other::CPtr<TriMesh> triMesh);
		virtual void Visit(CppUtility::Other::CPtr<Model> model);
	};
}

#endif // !_HITABLE_VISITOR_H_
