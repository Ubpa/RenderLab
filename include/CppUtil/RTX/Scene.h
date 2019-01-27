#ifndef _SCENE_H_
#define _SCENE_H_

#include <CppUtil/Basic/HeapObj.h>

namespace RTX {
	class Hitable;
	class RayCamera;

	class Scene : public CppUtil::Basic::HeapObj{
		HEAP_OBJ_SETUP(Scene)
	public:
		const Scene(CppUtil::Basic::CPtr<Hitable> obj, CppUtil::Basic::CPtr<RayCamera> camera);

		CppUtil::Basic::CPtr<Hitable> obj;
		const CppUtil::Basic::CPtr<RayCamera> camera;
	};
}

#endif // !_SCENE_H_
