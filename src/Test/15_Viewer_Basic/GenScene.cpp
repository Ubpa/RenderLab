#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Cube.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene() {
	auto sobj_Root = ToPtr(new SObj(nullptr));
	auto sobj_Camera = ToPtr(new SObj(sobj_Root, "camera"));
	auto sobj_MirrorSphere = ToPtr(new SObj(sobj_Root, "mirror sphere"));
	auto sobj_GlassSphere = ToPtr(new SObj(sobj_Root, "glass sphere"));
	auto sobj_AreaLight = ToPtr(new SObj(sobj_Root, "area light"));
	const int wallNum = 5;
	SObj::Ptr sobj_walls[wallNum] = {
		ToPtr(new SObj(sobj_Root, "wall ceil")),
		ToPtr(new SObj(sobj_Root, "wall down")),
		ToPtr(new SObj(sobj_Root, "wall left")),
		ToPtr(new SObj(sobj_Root, "wall right")),
		ToPtr(new SObj(sobj_Root, "wall back"))
	};

	// camera
	auto camera = ToPtr(new Camera(sobj_Camera));
	auto cameraTransform = ToPtr(new Transform(sobj_Camera));
	cameraTransform->SetPosition(vec3(0, 0.75f, 2.3f));

	// light
	auto areaLight = ToPtr(new AreaLight(vec3(1), 10, 0.8f, 0.6f));
	auto light = ToPtr(new Light(sobj_AreaLight, areaLight));
	auto lightTransform = ToPtr(new Transform(sobj_AreaLight));
	lightTransform->SetPosition(vec3(0, 1.49f, 0));
	lightTransform->SetScale(vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = ToPtr(new Plane);
	auto lightGeo = ToPtr(new Geometry(sobj_AreaLight, lightPlane));
	auto bsdfEmission = ToPtr(new BSDF_Emission(vec3(1)));
	auto materailEmission = ToPtr(new Material(sobj_AreaLight, bsdfEmission));

	// sphere
	auto sphere = ToPtr(new Sphere);

	// mirror sphere
	auto bsdfMirror = ToPtr(new BSDF_Mirror);
	auto materialMirror = ToPtr(new Material(sobj_MirrorSphere, bsdfMirror));

	auto mirrorSphereTransform = ToPtr(new Transform(sobj_MirrorSphere));
	mirrorSphereTransform->SetPosition(vec3(-0.4f, 0.3f, -0.3f));
	mirrorSphereTransform->SetScale(vec3(0.3f));

	auto geoMetalSphere = ToPtr(new Geometry(sobj_MirrorSphere, sphere));

	// glass sphere
	auto bsdfGlass = ToPtr(new BSDF_Glass(1.45f));
	auto materialGlass = ToPtr(new Material(sobj_GlassSphere, bsdfGlass));

	auto glassSphereTransform = ToPtr(new Transform(sobj_GlassSphere));
	glassSphereTransform->SetPosition(vec3(0.4f, 0.3f, 0.3f));
	glassSphereTransform->SetScale(vec3(0.3f));

	auto geoGlassSphere = ToPtr(new Geometry(sobj_GlassSphere, sphere));

	// wall
	vec3 posArr[wallNum] = {
		vec3(0, 1.5f, 0),
		vec3(0, 0, 0),
		vec3(-1, 0.75f, 0),
		vec3(1, 0.75f, 0),
		vec3(0, 0.75f, -1),
	};

	vec3 axisArr[wallNum] = {
		vec3(0, 1, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1),
		vec3(0, 0, 1),
		vec3(1, 0, 0),
	};

	float degreeArr[wallNum] = {
		0,
		0,
		90,
		90,
		90,
	};

	vec3 scaleArr[wallNum] = {
		vec3(2, 1, 2),
		vec3(2, 1, 2),
		vec3(1.5f, 1, 2),
		vec3(1.5f, 1, 2),
		vec3(2, 1, 1.5f),
	};

	vec3 colorArr[wallNum] = {
		vec3(0.6f),
		vec3(0.6f),
		vec3(0.6f, 0.2f, 0.2f),
		vec3(0.2f, 0.2f, 0.6f),
		vec3(0.6f),
	};

	for (int i = 0; i < wallNum; i++) {
		auto bsdfDiffuse = ToPtr(new BSDF_Diffuse(colorArr[i]));
		auto materialDiffuse = ToPtr(new Material(sobj_walls[i], bsdfDiffuse));

		auto plane = ToPtr(new Plane);
		auto geo = ToPtr(new Geometry(sobj_walls[i], plane));

		auto transform = ToPtr(new Transform(sobj_walls[i]));
		transform->SetPosition(posArr[i]);
		transform->Rotate(radians(degreeArr[i]), axisArr[i]);
		transform->SetScale(scaleArr[i]);
	}

	// sky sphere
	auto sobj_skySphere = ToPtr(new SObj(sobj_Root, "sky"));
	auto skySphere = ToPtr(new Sphere(vec3(0), 100.0f));
	auto geoSky = ToPtr(new Geometry(sobj_skySphere, skySphere));
	auto dark = ToPtr(new BSDF_Diffuse(vec3(0)));
	auto materialSky = ToPtr(new Material(sobj_skySphere, dark));

	auto scene = ToPtr(new Scene(sobj_Root));
	return scene;
}
