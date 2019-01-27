#include <CppUtil/RTX/Scene.h>

using namespace RTX;
using namespace CppUtil::Basic;

Scene::Scene(CppUtil::Basic::CPtr<Hitable> obj, CppUtil::Basic::CPtr<RayCamera> camera)
	: obj(obj), camera(camera) { }