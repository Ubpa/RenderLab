#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Cube.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

Scene::Ptr GenScene0() {
	auto sobj_Root = ToPtr(new SObj(nullptr, "root"));
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
	cameraTransform->SetPosition(vec3(0, 0.75f, 2.4f));
	camera->fov = 50;

	// light
	auto areaLight = ToPtr(new AreaLight(vec3(1), 15, 0.8f, 0.6f));
	auto light = ToPtr(new Light(sobj_AreaLight, areaLight));
	auto lightTransform = ToPtr(new Transform(sobj_AreaLight));
	lightTransform->SetPosition(vec3(0, 1.49f, 0));
	lightTransform->SetScale(vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = ToPtr(new Plane);
	auto lightGeo = ToPtr(new Geometry(sobj_AreaLight, lightPlane));
	auto bsdfEmission = ToPtr(new BSDF_Emission(vec3(2)));
	auto materailEmission = ToPtr(new Material(sobj_AreaLight, bsdfEmission));

	// mirror sphere
	auto bsdfMirror = ToPtr(new BSDF_Mirror);
	auto materialMirror = ToPtr(new Material(sobj_MirrorSphere, bsdfMirror));

	auto mirrorSphereTransform = ToPtr(new Transform(sobj_MirrorSphere));
	mirrorSphereTransform->SetPosition(vec3(-0.4f, 0.3f, -0.3f));
	mirrorSphereTransform->SetScale(vec3(0.3f));

	auto geoMetalSphere = ToPtr(new Geometry(sobj_MirrorSphere, ToPtr(new Sphere)));

	// glass sphere
	auto bsdfGlass = ToPtr(new BSDF_Glass(1.45f));
	auto materialGlass = ToPtr(new Material(sobj_GlassSphere, bsdfGlass));

	auto glassSphereTransform = ToPtr(new Transform(sobj_GlassSphere));
	glassSphereTransform->SetPosition(vec3(0.4f, 0.3f, 0.3f));
	glassSphereTransform->SetScale(vec3(0.3f));

	auto geoGlassSphere = ToPtr(new Geometry(sobj_GlassSphere, ToPtr(new Sphere)));

	// mesh . cube 
	auto sobj_Cube = ToPtr(new SObj(sobj_Root, "cube"));
	Cube cube;
	auto cubeMesh = ToPtr(new TriMesh(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr()));
	auto cubeG = ToPtr(new Geometry(sobj_Cube, cubeMesh));
	auto blueGlass = ToPtr(new BSDF_Glass(1.5f, vec3(0.f, 0.794f, 0.916f)));
	auto materialCube = ToPtr(new Material(sobj_Cube, blueGlass));
	auto cubeTransform = ToPtr(new Transform(sobj_Cube));
	cubeTransform->SetScale(vec3(0.2f));
	cubeTransform->SetPosition(vec3(0, 0.15f, 0));

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
	/*
	auto sobj_skySphere = ToPtr(new SObj(sobj_Root, "sky"));
	auto skySphere = ToPtr(new Sphere(vec3(0), 100.0f));
	auto geoSky = ToPtr(new Geometry(sobj_skySphere, skySphere));
	auto dark = ToPtr(new BSDF_Diffuse(vec3(0)));
	auto materialSky = ToPtr(new Material(sobj_skySphere, dark));
	*/

	auto scene = ToPtr(new Scene(sobj_Root));
	return scene;
}

Scene::Ptr GenScene1() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new Camera(sobjRoot));

	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, -2), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, -2), 99.5f));

	auto g0 = ToPtr(new Geometry(sobj0, sphere0));
	auto g1 = ToPtr(new Geometry(sobj1, sphere1));

	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));
	auto gray = ToPtr(new BSDF_Diffuse(vec3(0.8f)));

	auto material0 = ToPtr(new Material(sobj0, pink));
	auto material1 = ToPtr(new Material(sobj1, gray));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}

Scene::Ptr GenScene2() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new Camera(sobjRoot));

	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, -2), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, -2), 99.5f));

	auto g0 = ToPtr(new Geometry(sobj0, sphere0));
	auto g1 = ToPtr(new Geometry(sobj1, sphere1));

	auto white = ToPtr(new BSDF_Mirror(vec3(1.0f)));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new Material(sobj0, white));
	auto material1 = ToPtr(new Material(sobj1, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}

Scene::Ptr GenScene3(){
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new Camera(sobjRoot));

	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, -2), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, -2), 99.5f));

	auto g0 = ToPtr(new Geometry(sobj0, sphere0));
	auto g1 = ToPtr(new Geometry(sobj1, sphere1));

	auto glass = ToPtr(new BSDF_Glass(1.5f));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new Material(sobj0, glass));
	auto material1 = ToPtr(new Material(sobj1, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}

Scene::Ptr GenScene4() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));

	auto cameraTransform = ToPtr(new Transform(sobj_camera));
	cameraTransform->SetPosition(vec3(0, 0, 3));
	auto camera = ToPtr(new Camera(sobj_camera));

	auto sphere0 = ToPtr(new Sphere(vec3(0, 0, 0), 0.5f));
	auto sphere1 = ToPtr(new Sphere(vec3(0, -100, 0), 99.5f));

	auto sphereTransform = ToPtr(new Transform(sobj_sphere));
	sphereTransform->SetScale(vec3(2, 1, 1));
	sphereTransform->Rotate(radians(45.0f), vec3(0, 1, 0));

	auto g0 = ToPtr(new Geometry(sobj_sphere, sphere0));
	auto g1 = ToPtr(new Geometry(sobj_ground, sphere1));

	auto glass = ToPtr(new BSDF_Glass(1.2f));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new Material(sobj_sphere, glass));
	auto material1 = ToPtr(new Material(sobj_ground, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}

Scene::Ptr GenScene5() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));

	auto cameraTransform = ToPtr(new Transform(sobj_camera));
	cameraTransform->SetPosition(vec3(0, 0.1f, 3));
	auto camera = ToPtr(new Camera(sobj_camera));

	auto sphere = ToPtr(new Sphere(vec3(0, 0.5, 0), 0.5f));
	//auto plane = ToPtr(new Sphere(vec3(0, 0, 0), 1.f));
	auto plane = ToPtr(new Plane);

	auto sphereTransform = ToPtr(new Transform(sobj_sphere));
	sphereTransform->SetScale(vec3(1, 1, 1));
	sphereTransform->Rotate(radians(30.0f), vec3(0, 1, 0));

	auto planeTransform = ToPtr(new Transform(sobj_ground));
	planeTransform->SetScale(vec3(10.0f, 1.f, 10.0f));

	auto g0 = ToPtr(new Geometry(sobj_sphere, sphere));
	auto g1 = ToPtr(new Geometry(sobj_ground, plane));

	auto glass = ToPtr(new BSDF_Glass(1.2f));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new Material(sobj_sphere, glass));
	auto material1 = ToPtr(new Material(sobj_ground, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene; 
}

Scene::Ptr GenScene6() {
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
	auto glass = ToPtr(new BSDF_Glass(1.5f));
	auto pink = ToPtr(new BSDF_Diffuse(vec3(1.0f, 0.686f, 0.788f)));
	auto emission = ToPtr(new BSDF_Emission(vec3(1)));

	auto material0 = ToPtr(new Material(sobj_skySphere, sky));
	auto material1 = ToPtr(new Material(sobj_ground, pink));
	auto material2 = ToPtr(new Material(sobj_sphere, glass));
	auto material3 = ToPtr(new Material(sobj_light, emission));

	auto scene = ToPtr(new Scene(sobjRoot, "scene0"));
	return scene;
}

Scene::Ptr GenScene(int n) {
	const int num = 7;
	Scene::Ptr(*f[num])() = {
		&GenScene0, &GenScene1, &GenScene2, &GenScene3, &GenScene4, &GenScene5, &GenScene6
	};

	if (n < num)
		return f[n]();
	else
		return nullptr;
}
