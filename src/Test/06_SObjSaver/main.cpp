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
	auto sobj = SObj::New(nullptr, "root");
	auto sobj1 = SObj::New(sobj, "sobj1");
	auto sobj2 = SObj::New(sobj, "sobj2");
	auto sobj3 = SObj::New(sobj, "sobj3");
	auto sobj4 = SObj::New(sobj2, "sobj4");
	auto sobj5 = SObj::New(sobj, "sobj5");

	auto sobj30 = SObj::New(sobj3, "sobj30");
	auto sobj31 = SObj::New(sobj3, "sobj31");
	auto sobj32 = SObj::New(sobj3, "sobj32");
	auto sobj33 = SObj::New(sobj3, "sobj33");
	auto sobj34 = SObj::New(sobj3, "sobj34");
	auto sobj35 = SObj::New(sobj3, "sobj35");

	{
		auto camera = CmptCamera::New(sobj1, 3, 3, 3, 3);
		auto transform = CmptTransform::New(sobj1);
		transform->SetPosition(Vec3(3, 3, 3));
		transform->SetRotation(EulerYXZf(3, 3, 3).ToQuat());
		transform->SetScale(Vec3(3, 3, 3));
		auto light = CmptLight::New(sobj1, nullptr);
		auto material = CmptMaterial::New(sobj1, nullptr);
		auto geometry = CmptGeometry::New(sobj1, nullptr);
	}

	{
		auto geometry0 = CmptGeometry::New(sobj2, Sphere::New());
		auto geometry1 = CmptGeometry::New(sobj3, Plane::New());
		auto light = CmptLight::New(sobj2, AreaLight::New(Vec3(3),3.f,3.f,3.f));
	}

	{
		auto material0 = CmptMaterial::New(sobj30, BSDF_CookTorrance::New(3.f, 3.f, Vec3(3),Vec3(3)));
		auto material1 = CmptMaterial::New(sobj31, BSDF_Diffuse::New(Vec3(3.f)));
		auto material2 = CmptMaterial::New(sobj32, BSDF_Emission::New(Vec3(3.f),3.f));
		auto material3 = CmptMaterial::New(sobj33, BSDF_Glass::New(3.f,Vec3(3.f),Vec3(3.f)));
		auto material4 = CmptMaterial::New(sobj34, BSDF_MetalWorkflow::New(Vec3(3.f),3.f,3.f));
		auto material5 = CmptMaterial::New(sobj35, BSDF_Mirror::New(Vec3(3.f)));
	}

	{
		auto sobj_IronSphere = SObj::New(sobj5, "rusted iron sphere");

		auto bsdfIron = BSDF_MetalWorkflow::New(Vec3(3), 3.f, 3.f);
		auto ironAlbedoImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/albedo.png").c_str());
		bsdfIron->SetAlbedoTexture(ironAlbedoImg);
		auto ironMatallicImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/metallic.png").c_str());
		bsdfIron->SetMetallicTexture(ironMatallicImg);
		auto ironRoughnessImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/roughness.png").c_str());
		bsdfIron->SetRoughnessTexture(ironRoughnessImg);
		auto ironAOImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/ao.png").c_str());
		bsdfIron->SetAOTexture(ironAOImg);
		auto ironNormalImg = Image::New((ROOT_PATH + "data/textures/pbr/rusted_iron/normal.png").c_str());
		bsdfIron->SetNormalTexture(ironNormalImg);
		auto materialIron = CmptMaterial::New(sobj_IronSphere, bsdfIron);
	}

	sobj->Save(ROOT_PATH + "data/out/write.xml");

	auto newSObj = SObj::Load(ROOT_PATH + "data/out/write.xml");
	newSObj->Save(ROOT_PATH + "data/out/writeAfterRead.xml");

	return 0;
}
