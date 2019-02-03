#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Light.h>

using namespace CppUtil::Engine;
using namespace std;

namespace CppUtil {
	namespace Engine {

	}
}

Scene::Scene(SObj::Ptr rootSObj, const string & name)
	: rootSObj(rootSObj), name(name), mainCamera(nullptr) {

	if (rootSObj) {
		mainCamera = rootSObj->GetComponentInChildren<Camera>();
		lights = rootSObj->GetComponentsInChildren<Light>();
	}
}