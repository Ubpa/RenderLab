#include <Utility/RTX/Scene.h>

using namespace RayTracing;
using namespace CppUtility::Other;

Scene::Scene(CppUtility::Other::CPtr<Hitable> obj, CppUtility::Other::CPtr<RayCamera> camera)
	: obj(obj), camera(camera) { }