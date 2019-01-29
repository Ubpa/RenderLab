#ifndef _SCENE_CREATOR_H_
#define _SCENE_CREATOR_H_

#include <CppUtil/Basic/Ptr.h>

namespace RTX {
	class Scene;

	class SceneCreator {
	public:
		enum ENUM_SCENE
		{
			LOTS_OF_BALLS,
		};

		static CppUtil::Basic::CPtr<Scene> Gen(ENUM_SCENE scene, int w, int h);
	private:
		static CppUtil::Basic::CPtr<Scene> Gen_LOS_OF_BALLS(int w, int h);
	};
}

#endif // !_SCENE_CREATOR_H_
