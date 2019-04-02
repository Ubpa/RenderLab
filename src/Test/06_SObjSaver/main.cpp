#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/AllComponents.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

int main() {
	auto sobj = ToPtr(new SObj(nullptr, "root"));
	auto sobj1 = ToPtr(new SObj(sobj, "sobj1"));
	auto sobj2 = ToPtr(new SObj(sobj, "sobj2"));
	auto sobj3 = ToPtr(new SObj(sobj, "sobj3"));
	auto sobj4 = ToPtr(new SObj(sobj2, "sobj4"));
	auto sobj5 = ToPtr(new SObj(sobj, "sobj5"));

	auto sobj30 = ToPtr(new SObj(sobj3, "sobj30"));
	auto sobj31 = ToPtr(new SObj(sobj3, "sobj31"));
	auto sobj32 = ToPtr(new SObj(sobj3, "sobj32"));
	auto sobj33 = ToPtr(new SObj(sobj3, "sobj33"));
	auto sobj34 = ToPtr(new SObj(sobj3, "sobj34"));
	auto sobj35 = ToPtr(new SObj(sobj3, "sobj35"));

	{
		auto camera = ToPtr(new CmptCamera(sobj1, 3, 3, 3, 3));
		auto transform = ToPtr(new CmptTransform(sobj1));
		transform->SetPosition(Vec3(3, 3, 3));
		transform->SetRotation(EulerYXZf(3, 3, 3).ToQuat());
		transform->SetScale(Vec3(3, 3, 3));
		auto light = ToPtr(new CmptLight(sobj1, nullptr));
		auto material = ToPtr(new CmptMaterial(sobj1, nullptr));
		auto geometry = ToPtr(new CmptGeometry(sobj1, nullptr));
	}

	{
		auto geometry0 = ToPtr(new CmptGeometry(sobj2, ToPtr(new Sphere)));
		auto geometry1 = ToPtr(new CmptGeometry(sobj3, ToPtr(new Plane)));
		auto light = ToPtr(new CmptLight(sobj2, ToPtr(new AreaLight(Vec3(3),3.f,3.f,3.f))));
	}

	{
		auto material0 = ToPtr(new CmptMaterial(sobj30, ToPtr(new BSDF_CookTorrance(3.f, 3.f, Vec3(3),Vec3(3)))));
		auto material1 = ToPtr(new CmptMaterial(sobj31, ToPtr(new BSDF_Diffuse(Vec3(3.f)))));
		auto material2 = ToPtr(new CmptMaterial(sobj32, ToPtr(new BSDF_Emission(Vec3(3.f),3.f))));
		auto material3 = ToPtr(new CmptMaterial(sobj33, ToPtr(new BSDF_Glass(3.f,Vec3(3.f),Vec3(3.f)))));
		auto material4 = ToPtr(new CmptMaterial(sobj34, ToPtr(new BSDF_MetalWorkflow(Vec3(3.f),3.f,3.f))));
		auto material5 = ToPtr(new CmptMaterial(sobj35, ToPtr(new BSDF_Mirror(Vec3(3.f)))));
	}

	{
		auto sobj_IronSphere = ToPtr(new SObj(sobj5, "rusted iron sphere"));

		auto bsdfIron = ToPtr(new BSDF_MetalWorkflow(Vec3(3), 3.f, 3.f));
		auto ironAlbedoImg = ToPtr(new Image((ROOT_PATH + "data/textures/pbr/rusted_iron/albedo.png").c_str()));
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
	}

	sobj->Save(ROOT_PATH + "data/out/write.xml");

	auto newSObj = SObj::Load(ROOT_PATH + "data/out/write.xml");
	newSObj->Save(ROOT_PATH + "data/out/writeAfterRead.xml");

	return 0;
}
