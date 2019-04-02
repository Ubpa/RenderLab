#include "GenScene.h"

#include <CppUtil/Engine/Engine.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Cube.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

SObj::Ptr GenBox() {
	auto sobj_Box = ToPtr(new SObj(nullptr, "Cornell Box"));

	// wall
	const int wallNum = 5;
	SObj::Ptr sobj_walls[wallNum] = {
		ToPtr(new SObj(sobj_Box, "wall ceil")),
		ToPtr(new SObj(sobj_Box, "wall down")),
		ToPtr(new SObj(sobj_Box, "wall left")),
		ToPtr(new SObj(sobj_Box, "wall right")),
		ToPtr(new SObj(sobj_Box, "wall back"))
	};

	Point3 posArr[wallNum] = {
		Point3(0, 1.5f, 0),
		Point3(0, 0, 0),
		Point3(-1, 0.75f, 0),
		Point3(1, 0.75f, 0),
		Point3(0, 0.75f, -1),
	};

	Vec3 axisArr[wallNum] = {
		Vec3(0, 0, 1),
		Vec3(0, 1, 0),
		Vec3(0, 0, 1),
		Vec3(0, 0, 1),
		Vec3(1, 0, 0),
	};

	float degreeArr[wallNum] = {
		180,
		0,
		-90,
		90,
		90,
	};

	Vec3 scaleArr[wallNum] = {
		Vec3(2, 1, 2),
		Vec3(2, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(2, 1, 1.5f),
	};

	RGBf colorArr[wallNum] = {
		RGBf(0.6f),
		RGBf(0.6f),
		RGBf(0.6f, 0.2f, 0.2f),
		RGBf(0.2f, 0.2f, 0.6f),
		RGBf(0.6f),
	};

	for (int i = 0; i < wallNum; i++) {
		auto bsdfDiffuse = ToPtr(new BSDF_Diffuse(colorArr[i]));
		auto materialDiffuse = ToPtr(new CmptMaterial(sobj_walls[i], bsdfDiffuse));

		auto plane = ToPtr(new Plane);
		auto geo = ToPtr(new CmptGeometry(sobj_walls[i], plane));

		auto transform = ToPtr(new CmptTransform(sobj_walls[i]));
		transform->SetPosition(posArr[i]);
		transform->SetRotation(axisArr[i], degreeArr[i]);
		transform->SetScale(scaleArr[i]);
	}

	// light
	auto sobj_AreaLight = ToPtr(new SObj(sobj_Box, "area light"));
	auto areaLight = ToPtr(new AreaLight(RGBf(1), 15, 0.8f, 0.6f));
	auto light = ToPtr(new CmptLight(sobj_AreaLight, areaLight));
	auto lightTransform = ToPtr(new CmptTransform(sobj_AreaLight));
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = ToPtr(new Plane);
	auto lightGeo = ToPtr(new CmptGeometry(sobj_AreaLight, lightPlane));
	auto bsdfEmission = ToPtr(new BSDF_Emission(RGBf(2)));
	auto materailEmission = ToPtr(new CmptMaterial(sobj_AreaLight, bsdfEmission));

	return sobj_Box;
}

SObj::Ptr GenGound() {
	auto sobj_ground = ToPtr(new SObj(nullptr, "ground"));
	auto material = ToPtr(new CmptMaterial(sobj_ground, ToPtr(new BSDF_Diffuse)));
	auto plane = ToPtr(new CmptGeometry(sobj_ground, ToPtr(new Plane)));
	auto transform = ToPtr(new CmptTransform(sobj_ground));
	transform->SetScale(Vec3(100, 1, 100));
	return sobj_ground;
}

Scene::Ptr GenScene00() {
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
	auto camera = ToPtr(new CmptCamera(sobj_Camera, 50.0f));
	auto cameraTransform = ToPtr(new CmptTransform(sobj_Camera));
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// light
	auto areaLight = ToPtr(new AreaLight(RGBf(1), 15, 0.8f, 0.6f));
	auto light = ToPtr(new CmptLight(sobj_AreaLight, areaLight));
	auto lightTransform = ToPtr(new CmptTransform(sobj_AreaLight));
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = ToPtr(new Plane);
	auto lightGeo = ToPtr(new CmptGeometry(sobj_AreaLight, lightPlane));
	auto bsdfEmission = ToPtr(new BSDF_Emission(RGBf(2)));
	auto materailEmission = ToPtr(new CmptMaterial(sobj_AreaLight, bsdfEmission));

	// mirror sphere
	auto bsdfMirror = ToPtr(new BSDF_Mirror);
	auto materialMirror = ToPtr(new CmptMaterial(sobj_MirrorSphere, bsdfMirror));

	auto mirrorSphereTransform = ToPtr(new CmptTransform(sobj_MirrorSphere));
	mirrorSphereTransform->SetPosition(Point3(-0.4f, 0.3f, -0.3f));
	mirrorSphereTransform->SetScale(Vec3(0.3f));

	auto geoMetalSphere = ToPtr(new CmptGeometry(sobj_MirrorSphere, ToPtr(new Sphere)));

	// glass sphere
	auto bsdfGlass = ToPtr(new BSDF_Glass(1.45f));
	auto materialGlass = ToPtr(new CmptMaterial(sobj_GlassSphere, bsdfGlass));

	auto glassSphereTransform = ToPtr(new CmptTransform(sobj_GlassSphere));
	glassSphereTransform->SetPosition(Point3(0.4f, 0.3f, 0.3f));
	glassSphereTransform->SetScale(Vec3(0.3f));

	auto geoGlassSphere = ToPtr(new CmptGeometry(sobj_GlassSphere, ToPtr(new Sphere)));

	// mesh . cube 
	auto sobj_Cube = ToPtr(new SObj(sobj_Root, "cube"));
	Cube cube;
	auto cubeMesh = ToPtr(new TriMesh(cube.GetTriNum(), cube.GetVertexNum(),
		cube.GetIndexArr(), cube.GetPosArr(), cube.GetNormalArr(), cube.GetTexCoordsArr()));
	auto cubeG = ToPtr(new CmptGeometry(sobj_Cube, cubeMesh));
	auto blueGlass = ToPtr(new BSDF_Glass(1.5f, RGBf(0.f, 0.794f, 0.916f)));
	auto materialCube = ToPtr(new CmptMaterial(sobj_Cube, blueGlass));
	auto cubeTransform = ToPtr(new CmptTransform(sobj_Cube));
	cubeTransform->SetScale(Vec3(0.2f));
	cubeTransform->SetPosition(Point3(0, 0.15f, 0));

	GenBox()->SetParent(sobj_Root);

	auto scene = ToPtr(new Scene(sobj_Root, "scene 00"));
	return scene;
}

Scene::Ptr GenScene01() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new CmptCamera(sobjRoot));

	auto sphere0Transform = ToPtr(new CmptTransform(sobj0, Point3(0,0,-2), Vec3(0.5f)));
	auto sphere0 = ToPtr(new Sphere);
	auto sphere1Transform = ToPtr(new CmptTransform(sobj0, Point3(0, -100, -2), Vec3(99.5f)));
	auto sphere1 = ToPtr(new Sphere);

	auto g0 = ToPtr(new CmptGeometry(sobj0, sphere0));
	auto g1 = ToPtr(new CmptGeometry(sobj1, sphere1));

	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));
	auto gray = ToPtr(new BSDF_Diffuse(RGBf(0.8f)));

	auto material0 = ToPtr(new CmptMaterial(sobj0, pink));
	auto material1 = ToPtr(new CmptMaterial(sobj1, gray));

	auto scene = ToPtr(new Scene(sobjRoot, "scene 01"));
	return scene;
}

Scene::Ptr GenScene02() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new CmptCamera(sobjRoot));

	auto sphere0Transform = ToPtr(new CmptTransform(sobj0, Point3(0, 0, -2), Vec3(0.5f)));
	auto sphere0 = ToPtr(new Sphere);
	auto sphere1Transform = ToPtr(new CmptTransform(sobj0, Point3(0, -100, -2), Vec3(99.5f)));
	auto sphere1 = ToPtr(new Sphere);

	auto g0 = ToPtr(new CmptGeometry(sobj0, sphere0));
	auto g1 = ToPtr(new CmptGeometry(sobj1, sphere1));

	auto white = ToPtr(new BSDF_Mirror(RGBf(1.0f)));
	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new CmptMaterial(sobj0, white));
	auto material1 = ToPtr(new CmptMaterial(sobj1, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene 02"));
	return scene;
}

Scene::Ptr GenScene03(){
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj0 = ToPtr(new SObj(sobjRoot, "sobj0"));
	auto sobj1 = ToPtr(new SObj(sobjRoot, "sobj1"));

	auto camera = ToPtr(new CmptCamera(sobjRoot));

	auto sphere0Transform = ToPtr(new CmptTransform(sobj0, Point3(0, 0, -2), Vec3(0.5f)));
	auto sphere0 = ToPtr(new Sphere);
	auto sphere1Transform = ToPtr(new CmptTransform(sobj0, Point3(0, -100, -2), Vec3(99.5f)));
	auto sphere1 = ToPtr(new Sphere);

	auto g0 = ToPtr(new CmptGeometry(sobj0, sphere0));
	auto g1 = ToPtr(new CmptGeometry(sobj1, sphere1));

	auto glass = ToPtr(new BSDF_Glass(1.5f));
	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new CmptMaterial(sobj0, glass));
	auto material1 = ToPtr(new CmptMaterial(sobj1, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene 03"));
	return scene;
}

Scene::Ptr GenScene04() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));

	auto cameraTransform = ToPtr(new CmptTransform(sobj_camera));
	cameraTransform->SetPosition(Point3(0, 0, 3));
	auto camera = ToPtr(new CmptCamera(sobj_camera));

	auto sphere0Transform = ToPtr(new CmptTransform(sobj_sphere, Point3(0, 0, 0), Vec3(1, 0.5,0.5), Vec3(0,1,0), 45));
	auto sphere0 = ToPtr(new Sphere);
	auto sphere1Transform = ToPtr(new CmptTransform(sobj_ground, Point3(0, -100, 0), Vec3(99.5f)));
	auto sphere1 = ToPtr(new Sphere);

	auto g0 = ToPtr(new CmptGeometry(sobj_sphere, sphere0));
	auto g1 = ToPtr(new CmptGeometry(sobj_ground, sphere1));

	auto glass = ToPtr(new BSDF_Glass(1.2f));
	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new CmptMaterial(sobj_sphere, glass));
	auto material1 = ToPtr(new CmptMaterial(sobj_ground, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene 04"));
	return scene;
}

Scene::Ptr GenScene05() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));

	auto cameraTransform = ToPtr(new CmptTransform(sobj_camera));
	cameraTransform->SetPosition(Point3(0, 0.1f, 3));
	auto camera = ToPtr(new CmptCamera(sobj_camera));

	auto sphereTransform = ToPtr(new CmptTransform(sobj_sphere, Point3(0, 0, -2), Vec3(0.5f), Vec3(0,1,0), 30));
	auto sphere = ToPtr(new Sphere);
	auto plane = ToPtr(new Plane);

	auto planeTransform = ToPtr(new CmptTransform(sobj_ground));
	planeTransform->SetScale(Vec3(10.0f, 1.f, 10.0f));

	auto g0 = ToPtr(new CmptGeometry(sobj_sphere, sphere));
	auto g1 = ToPtr(new CmptGeometry(sobj_ground, plane));

	auto glass = ToPtr(new BSDF_Glass(1.2f));
	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));

	auto material0 = ToPtr(new CmptMaterial(sobj_sphere, glass));
	auto material1 = ToPtr(new CmptMaterial(sobj_ground, pink));

	auto scene = ToPtr(new Scene(sobjRoot, "scene05"));
	return scene; 
}

Scene::Ptr GenScene06() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));
	auto sobj_camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto sobj_skySphere = ToPtr(new SObj(sobjRoot, "sobj_skySphere"));
	auto sobj_sphere = ToPtr(new SObj(sobjRoot, "sobj_sphere"));
	auto sobj_light = ToPtr(new SObj(sobjRoot, "sobj_light"));
	auto sobj_ground = ToPtr(new SObj(sobjRoot, "sobj_ground"));

	auto cameraTransform = ToPtr(new CmptTransform(sobj_camera));
	cameraTransform->SetPosition(Point3(0, 0.5f, 3));
	auto camera = ToPtr(new CmptCamera(sobj_camera));

	auto skySphereTransform = ToPtr(new CmptTransform(sobj_sphere, Point3(0), Vec3(4)));
	auto skySphere = ToPtr(new Sphere);
	auto sphereTransform = ToPtr(new CmptTransform(sobj_sphere, Point3(0), Vec3(0.5f)));
	auto sphere = ToPtr(new Sphere);
	auto plane = ToPtr(new Plane);
	auto lightPlane = ToPtr(new Plane);
	auto areaLight = ToPtr(new AreaLight(RGBf(1), 10));

	auto planeTransform = ToPtr(new CmptTransform(sobj_ground));
	planeTransform->SetScale(Vec3(100.0f, 1.f, 100.0f));
	planeTransform->SetPosition(Point3(0, -1, 0));

	auto lightTransform = ToPtr(new CmptTransform(sobj_light));
	lightTransform->SetPosition(Point3(0, 2.f, 0));

	auto light = ToPtr(new CmptLight(sobj_light, areaLight));

	auto g0 = ToPtr(new CmptGeometry(sobj_skySphere, skySphere));
	auto g1 = ToPtr(new CmptGeometry(sobj_ground, plane));
	auto g2 = ToPtr(new CmptGeometry(sobj_sphere, sphere));
	auto g3 = ToPtr(new CmptGeometry(sobj_light, lightPlane));

	auto sky = ToPtr(new BSDF_Diffuse(RGBf(0)));
	auto glass = ToPtr(new BSDF_Glass(1.5f));
	auto pink = ToPtr(new BSDF_Diffuse(RGBf(1.0f, 0.686f, 0.788f)));
	auto emission = ToPtr(new BSDF_Emission(RGBf(1)));

	auto material0 = ToPtr(new CmptMaterial(sobj_skySphere, sky));
	auto material1 = ToPtr(new CmptMaterial(sobj_ground, pink));
	auto material2 = ToPtr(new CmptMaterial(sobj_sphere, glass));
	auto material3 = ToPtr(new CmptMaterial(sobj_light, emission));

	auto scene = ToPtr(new Scene(sobjRoot, "scene 06"));
	return scene;
}

Scene::Ptr GenScene07() {
	auto sobjRoot = ToPtr(new SObj(nullptr, "root"));

	auto sobj_Camera = ToPtr(new SObj(sobjRoot, "camera"));
	auto camera = ToPtr(new CmptCamera(sobj_Camera, 20.0f));
	auto cameraTransform = ToPtr(new CmptTransform(sobj_Camera));
	cameraTransform->LookAt(Point3(13, 2, 3), Point3(0));

	auto balls = ToPtr(new SObj(sobjRoot, "balls"));
	for (int a = -11, id = 0; a < 11; a++) {
		for (int b = -11; b < 11; b++, id++) {
			auto ball = ToPtr(new SObj(balls, "ball " + to_string(id)));
			Point3 center(a + 0.9*Math::Rand_F(), 0.2, b + 0.9*Math::Rand_F());
			auto tsfm = ToPtr(new CmptTransform(ball, center, Vec3(0.2f)));

			auto geo = ToPtr(new CmptGeometry(ball, ToPtr(new Sphere)));

			BSDF::Ptr bsdf;
			float choose_mat = Math::Rand_F();
			if (choose_mat < 0.8) {  // diffuse
				RGBf color(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F());
				bsdf = ToPtr(new BSDF_Diffuse(color));
			}
			else if (choose_mat < 0.95) { // metal
				RGBf color(0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()), 0.5f*(1 + Math::Rand_F()));
				bsdf = ToPtr(new BSDF_Mirror(color));
			}
			else {  // glass
				bsdf = ToPtr(new BSDF_Glass(1.2f + Math::Rand_F()*0.5f));
			}

			auto material = ToPtr(new CmptMaterial(ball, bsdf));
		}
	}


	Point3 center[6] = {
		Point3(0, -1000, 0),
		Point3(6, 1, 0),
		Point3(2, 1, 0),
		Point3(2, 1, 0),
		Point3(-2, 1, 0),
		Point3(-6, 1, 0),
	};
	float radius[6] = { 1000.f,1.f,1.f,-0.8f,1.f,1.f };
	BSDF::Ptr bsdf[6] = {
		ToPtr(new BSDF_Diffuse(RGBf(0.5f))),
		ToPtr(new BSDF_Mirror(RGBf(0.7f, 0.6f, 0.5f))),
		ToPtr(new BSDF_Glass(1.5f)),
		ToPtr(new BSDF_Glass(1.5f)),
		ToPtr(new BSDF_Diffuse(RGBf(0.4f,0.2f,0.1f))),
		ToPtr(new BSDF_Glass(2.5f)),
	};
	for (int i = 0; i < 6; i++) {
		auto sobj = ToPtr(new SObj(sobjRoot, "sphere" + to_string(i)));
		auto tsfm = ToPtr(new CmptTransform(sobj, center[i], Vec3(radius[i])));
		auto geo = ToPtr(new CmptGeometry(sobj, ToPtr(new Sphere)));
		auto material = ToPtr(new CmptMaterial(sobj, bsdf[i]));
	}

	return ToPtr(new Scene(sobjRoot, "scene 07"));
}

Scene::Ptr GenScene08() {
	auto sobj_Root = ToPtr(new SObj(nullptr, "root"));

	// camera
	auto sobj_Camera = ToPtr(new SObj(sobj_Root, "camera"));
	auto camera = ToPtr(new CmptCamera(sobj_Camera, 50.0f));
	auto cameraTransform = ToPtr(new CmptTransform(sobj_Camera));
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// light
	auto sobj_AreaLight = ToPtr(new SObj(sobj_Root, "area light"));
	auto areaLight = ToPtr(new AreaLight(RGBf(1), 15, 0.8f, 0.6f));
	auto light = ToPtr(new CmptLight(sobj_AreaLight, areaLight));
	auto lightTransform = ToPtr(new CmptTransform(sobj_AreaLight));
	lightTransform->SetPosition(Point3(0, 1.49f, 0));
	lightTransform->SetScale(Vec3(0.8f, 1.0f, 0.6f));
	auto lightPlane = ToPtr(new Plane);
	auto lightGeo = ToPtr(new CmptGeometry(sobj_AreaLight, lightPlane));
	auto bsdfEmission = ToPtr(new BSDF_Emission(RGBf(2)));
	auto materailEmission = ToPtr(new CmptMaterial(sobj_AreaLight, bsdfEmission));

	// wall

	const int wallNum = 5;
	SObj::Ptr sobj_walls[wallNum] = {
		ToPtr(new SObj(sobj_Root, "wall ceil")),
		ToPtr(new SObj(sobj_Root, "wall down")),
		ToPtr(new SObj(sobj_Root, "wall left")),
		ToPtr(new SObj(sobj_Root, "wall right")),
		ToPtr(new SObj(sobj_Root, "wall back"))
	};

	Point3 posArr[wallNum] = {
		Point3(0, 1.5f, 0),
		Point3(0, 0, 0),
		Point3(-1, 0.75f, 0),
		Point3(1, 0.75f, 0),
		Point3(0, 0.75f, -1),
	};

	Vec3 axisArr[wallNum] = {
		Vec3(0, 1, 0),
		Vec3(0, 1, 0),
		Vec3(0, 0, 1),
		Vec3(0, 0, 1),
		Vec3(1, 0, 0),
	};

	float degreeArr[wallNum] = {
		0,
		0,
		90,
		90,
		90,
	};

	Vec3 scaleArr[wallNum] = {
		Vec3(2, 1, 2),
		Vec3(2, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(1.5f, 1, 2),
		Vec3(2, 1, 1.5f),
	};

	RGBf colorArr[wallNum] = {
		RGBf(0.6f),
		RGBf(0.6f),
		RGBf(0.6f, 0.2f, 0.2f),
		RGBf(0.2f, 0.2f, 0.6f),
		RGBf(0.6f),
	};

	for (int i = 0; i < wallNum; i++) {
		auto bsdfDiffuse = ToPtr(new BSDF_Diffuse(colorArr[i]));
		auto materialDiffuse = ToPtr(new CmptMaterial(sobj_walls[i], bsdfDiffuse));

		auto plane = ToPtr(new Plane);
		auto geo = ToPtr(new CmptGeometry(sobj_walls[i], plane));

		auto transform = ToPtr(new CmptTransform(sobj_walls[i], posArr[i], scaleArr[i], axisArr[i], degreeArr[i]));
	}

	// cook torrance sphere
	auto sobj_CTSphere = ToPtr(new SObj(sobj_Root, "cook torrance sphere"));

	auto bsdfCookTorrance = ToPtr(new BSDF_CookTorrance(10.f, 0.4f, RGBf(0.1f), RGBf(1.0f,0.3f,0.5f)));
	auto materialCookTorrance = ToPtr(new CmptMaterial(sobj_CTSphere, bsdfCookTorrance));

	auto CTSphereTransform = ToPtr(new CmptTransform(sobj_CTSphere));
	CTSphereTransform->SetPosition(Point3(0, 0.3f, 0));
	CTSphereTransform->SetScale(Vec3(0.3f));

	auto geoCTSphere = ToPtr(new CmptGeometry(sobj_CTSphere, ToPtr(new Sphere)));

	// sky sphere
	auto sobj_skySphere = ToPtr(new SObj(sobj_Root, "sky"));
	auto skyTsfm = ToPtr(new CmptTransform(sobj_skySphere, Point3(0), Vec3(100)));
	auto geoSky = ToPtr(new CmptGeometry(sobj_skySphere, ToPtr(new Sphere)));
	auto dark = ToPtr(new BSDF_Diffuse(RGBf(0)));
	auto materialSky = ToPtr(new CmptMaterial(sobj_skySphere, dark));

	return ToPtr(new Scene(sobj_Root, "scene 08"));
}

Scene::Ptr GenScene09() {
	auto sobj_Root = ToPtr(new SObj(nullptr, "root"));

	// metal workflow sphere
	auto sobj_MWSphere = ToPtr(new SObj(sobj_Root, "metal workflow sphere"));

	RGBf gold(1.00, 0.71, 0.29);
	auto bsdfGold = ToPtr(new BSDF_MetalWorkflow(gold, 0.2f));
	auto materialGold = ToPtr(new CmptMaterial(sobj_MWSphere, bsdfGold));

	auto MWSphereTransform = ToPtr(new CmptTransform(sobj_MWSphere));
	MWSphereTransform->SetPosition(Point3(0, 0.3f, 0));
	MWSphereTransform->SetScale(Vec3(0.3f));

	auto geoMWSphere = ToPtr(new CmptGeometry(sobj_MWSphere, ToPtr(new Sphere)));

	// cornell box
	auto box = GenBox();
	auto boxTransform = ToPtr(new CmptTransform(box));
	box->SetParent(sobj_Root);

	// ground
	//auto ground = GenGound();
	//ground->SetParent(sobj_Root);

	// plane

	// camera
	auto sobj_Camera = ToPtr(new SObj(sobj_Root, "camera"));
	auto camera = ToPtr(new CmptCamera(sobj_Camera, 50.0f));
	auto cameraTransform = ToPtr(new CmptTransform(sobj_Camera));
	cameraTransform->SetPosition(Point3(0, 0.75f, 2.4f));

	// sky sphere
	auto sobj_skySphere = ToPtr(new SObj(sobj_Root, "sky"));
	auto skyTsfm = ToPtr(new CmptTransform(sobj_skySphere, Point3(0), Vec3(100)));
	auto geoSky = ToPtr(new CmptGeometry(sobj_skySphere, ToPtr(new Sphere)));
	auto dark = ToPtr(new BSDF_Diffuse(RGBf(0)));
	auto materialSky = ToPtr(new CmptMaterial(sobj_skySphere, dark));

	return ToPtr(new Scene(sobj_Root, "scene 09"));
}

Scene::Ptr GenScene10() {
	auto sobj_Root = ToPtr(new SObj(nullptr, "root"));

	// rusted iron workflow sphere
	auto sobj_IronSphere = ToPtr(new SObj(sobj_Root, "rusted iron sphere"));

	auto bsdfIron = ToPtr(new BSDF_MetalWorkflow(RGBf(1)));
	auto ironAlbedoImg = ToPtr(new Image((ROOT_PATH+"data/textures/pbr/rusted_iron/albedo.png").c_str()));
	bsdfIron->SetAlbedoTexture(ironAlbedoImg);
	auto ironMatallicImg = ToPtr(new Image((ROOT_PATH + "data/textures/pbr/rusted_iron/metallic.png").c_str()));
	bsdfIron->SetMetallicTexture(ironMatallicImg);
	auto ironRoughnessImg = ToPtr(new Image((ROOT_PATH + "data/textures/pbr/rusted_iron/roughness.png").c_str()));
	bsdfIron->SetRoughnessTexture(ironRoughnessImg);
	auto ironAOImg = ToPtr(new Image((ROOT_PATH + "data/textures/pbr/rusted_iron/ao.png").c_str()));
	bsdfIron->SetAOTexture(ironAOImg);
	auto ironNormalImg = ToPtr(new Image((ROOT_PATH + "data/textures/pbr/rusted_iron/normal.png").c_str()));
	bsdfIron->SetNormalTexture(ironNormalImg);
	auto materialIron = ToPtr(new CmptMaterial(sobj_IronSphere, bsdfIron));

	auto MWSphereTransform = ToPtr(new CmptTransform(sobj_IronSphere, Point3(0, 0.3f, 0), Vec3(0.3f)));
	MWSphereTransform->Rotate(Vec3(0, 1, 0), 90.f);
	MWSphereTransform->Rotate(Vec3(1, 0, 0), 180.f);

	auto geoMWSphere = ToPtr(new CmptGeometry(sobj_IronSphere, ToPtr(new Sphere)));

	// cornell box
	auto box = GenBox();
	auto boxTransform = ToPtr(new CmptTransform(box));
	box->SetParent(sobj_Root);

	// ground
	//auto ground = GenGound();
	//ground->SetParent(sobj_Root);

	// plane

	// camera
	auto sobj_Camera = ToPtr(new SObj(sobj_Root, "camera"));
	auto camera = ToPtr(new CmptCamera(sobj_Camera, 50.0f));
	auto cameraTransform = ToPtr(new CmptTransform(sobj_Camera, Point3(0, 0.5f, 0.8f)));
	cameraTransform->SetPosition(Point3(0, 0.5f, 0.8f));
	cameraTransform->Rotate(Vec3(1, 0, 0), -15.f);

	// sky sphere
	auto sobj_skySphere = ToPtr(new SObj(sobj_Root, "sky"));
	auto skyTsfm = ToPtr(new CmptTransform(sobj_skySphere, Point3(0), Vec3(100)));
	auto geoSky = ToPtr(new CmptGeometry(sobj_skySphere, ToPtr(new Sphere)));
	auto dark = ToPtr(new BSDF_Diffuse(RGBf(0)));
	auto materialSky = ToPtr(new CmptMaterial(sobj_skySphere, dark));

	return ToPtr(new Scene(sobj_Root, "scene 10"));
}

Scene::Ptr GenScene(int n) {
	const int num = 11;
	Scene::Ptr(*f[num])() = {
		&GenScene00,
		&GenScene01,
		&GenScene02,
		&GenScene03,
		&GenScene04,
		&GenScene05,
		&GenScene06,
		&GenScene07,
		&GenScene08,
		&GenScene09,
		&GenScene10,
	};

	if (n < num)
		return f[n]();
	else
		return nullptr;
}
