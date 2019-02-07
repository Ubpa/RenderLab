#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_skySphere = ToPtr(new SObj(sobjRoot, "sobj_skySphere"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_light = ToPtr(new SObj(sobjRoot, "sobj_light"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));
	
	auto cameraTransform = ToPtr(new Transform(sobj_camera));
	cameraTransform->SetPosition(vec3(0, 0.5f, 3));
	auto camera = ToPtr(new Camera(sobj_camera));
	
	auto skySphere = ToPtr(new Sphere(vec3(0), 4.f));
	auto sphere = ToPtr(new Sphere(vec3(0), 0.5f));
	auto plane = ToPtr(new Plane);
	auto lightPlane = ToPtr(new Plane);
	auto areaLight = ToPtr(new AreaLight(vec3(1), 10));

	auto skySphereTransform = ToPtr(new Transform(sobj_skySphere));
	skySphereTransform->SetScale(vec3(1, 1, 1));
	skySphereTransform->Rotate(radians(30.0f), vec3(0, 1, 0));

	auto planeTransform = ToPtr(new Transform(sobj_ground));
	planeTransform->SetScale(vec3(100.0f, 1.f, 100.0f));
	planeTransform->SetPosition(vec3(0, -1, 0));

	auto lightTransform = ToPtr(new Transform(sobj_light));
	lightTransform->SetPosition(vec3(0, 2.f, 0));

	auto light = ToPtr(new Light(sobj_light, areaLight));

	auto g0 = ToPtr(new Geometry(sobj_skySphere, skySphere));
	auto g1 = ToPtr(new Geometry(sobj_ground, plane));
	auto g2 = ToPtr(new Geometry(sobj_sphere, sphere));
	auto g3 = ToPtr(new Geometry(sobj_light, lightPlane));

	auto sky = ToPtr(new BSDF_Diffuse(vec3(0)));
	auto glass = ToPtr(new BSDF_Mirror);
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));
	auto emission = ToPtr(new BSDF_Emission(vec3(1)));

	auto material0 = ToPtr(new Material(sobj_skySphere, sky));
	auto material1 = ToPtr(new Material(sobj_ground, pink));
	auto material2 = ToPtr(new Material(sobj_sphere, glass));
	auto material3 = ToPtr(new Material(sobj_light, emission));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}
