#ifndef _SCENE_H_
#define _SCENE_H_

#include <Utility/Basic/HeapObj.h>

namespace RayTracing {
	class Hitable;
	class RayCamera;

	class Scene : public CppUtility::Other::HeapObj{
		HEAP_OBJ_SETUP(Scene)
	public:
		const Scene(CppUtility::Other::CPtr<Hitable> obj, CppUtility::Other::CPtr<RayCamera> camera);

		CppUtility::Other::CPtr<Hitable> obj;
		const CppUtility::Other::CPtr<RayCamera> camera;
	};
}

#endif // !_SCENE_H_
