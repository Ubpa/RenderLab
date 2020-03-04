#ifndef _GEN_SCENE_H_
#define _GEN_SCENE_H_

#include <Basic/Ptr.h>

namespace CppUtil {
	namespace Engine {
		class Scene;
	}
}

CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GenScene(int n);

#endif//!_GEN_SCENE_H_
