#include "SObjLoader.h"

#include "SL_Common.h"

#include <CppUtil/Basic/Transform.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace tinyxml2;
using namespace glm;
using namespace std;

template<>
const float SObjLoader::To(const Key & key, float *) {
	return static_cast<float>(atof(key.c_str()));
}

template<>
const int SObjLoader::To(const Key & key, int *) {
	return atoi(key.c_str());
}

template<>
const string SObjLoader::To(const Key & key, string *) {
	return key;
}

template<>
const Image::Ptr SObjLoader::To(const Key & key, Image::Ptr *) {
	return ToPtr(new Image(key));
}

// ------------ Basic ----------------

void SObjLoader::LoadNode(EleP ele, const FuncMap & funcMap) {
	for (auto child = ele->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		auto target = funcMap.find(child->Name());
		if (target != funcMap.cend())
			target->second(child);
	}
}

template<>
static Image::Ptr SObjLoader::Load(EleP ele, Image::Ptr*) {
	FuncMap funcMap;

	Image::Ptr img = nullptr;

	Reg_Text_val(funcMap, str::Image::path, img);

	LoadNode(ele, funcMap);

	return img;
}

// ------------ SObj ----------------

template<>
static SObj::Ptr SObjLoader::Load(XMLElement * ele, SObj::Ptr*) {
	if (ele == nullptr)
		return nullptr;

	auto sobj = ToPtr(new SObj(nullptr));

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::SObj::name, sobj->name);
	Reg_Load_Lambda(funcMap, str::SObj::components, [&](const vector<Component::Ptr> & components) {
		for (const auto cmpt : components)
			cmpt->AttachSObj(sobj);
	});
	Reg_Load_Lambda(funcMap, str::SObj::children, [&](const vector<SObj::Ptr> & children) {
		for (const auto child : children)
			child->SetParent(sobj);
	});

	LoadNode(ele, funcMap);

	return sobj;
}

SObj::Ptr SObjLoader::Load(const string & path) {
	XMLDocument doc;

	if (doc.LoadFile(path.c_str()) != XML_SUCCESS)
		return nullptr;

	auto tmp = ToPtr(new SObj(nullptr));

	return Load<SObj::Ptr>(doc.FirstChildElement(str::SObj::type));
}

template<>
static vector<Component::Ptr> SObjLoader::Load(XMLElement * ele, vector<Component::Ptr>*) {
	FuncMap funcMap;

	vector<Component::Ptr> cmpts;

	Reg_Load_Lambda(funcMap, str::CmptCamera::type, [&](const CmptCamera::Ptr & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptGeometry::type, [&](const CmptGeometry::Ptr & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptLight::type, [&](const CmptLight::Ptr & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptMaterial::type, [&](const CmptMaterial::Ptr & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptTransform::type, [&](const CmptTransform::Ptr & cmpt) {
		cmpts.push_back(cmpt);
	});

	LoadNode(ele, funcMap);

	return cmpts;
}

template<>
static vector<SObj::Ptr> SObjLoader::Load(XMLElement * ele, vector<SObj::Ptr>*) {
	vector<SObj::Ptr> children;

	FuncMap funcMap;

	Reg_Load_Lambda(funcMap, str::SObj::type, [&](const SObj::Ptr & child) {
		children.push_back(child);
	});

	LoadNode(ele, funcMap);

	return children;
}

// ------------ Camera ----------------

template<>
static CmptCamera::Ptr SObjLoader::Load(XMLElement * ele, CmptCamera::Ptr*){
	auto cmpt = ToPtr(new CmptCamera(nullptr));

	FuncMap funcMap;
	Reg_Text_setVal(funcMap, str::CmptCamera::fov, cmpt, &CmptCamera::SetFOV);
	Reg_Text_setVal(funcMap, str::CmptCamera::ar, cmpt, &CmptCamera::SetAspectRatio);
	Reg_Text_val(funcMap, str::CmptCamera::nearPlane, cmpt->nearPlane);
	Reg_Text_val(funcMap, str::CmptCamera::farPlane, cmpt->farPlane);

	LoadNode(ele, funcMap);

	return cmpt;
}

// ------------ Geometry ----------------

template<>
static CmptGeometry::Ptr SObjLoader::Load(XMLElement * ele, CmptGeometry::Ptr*){
	auto geometry = ToPtr(new CmptGeometry(nullptr, nullptr));

	FuncMap funcMap;
	Reg_Load_val<Primitive::Ptr>(funcMap, str::CmptGeometry::primitive, geometry->primitive);

	LoadNode(ele, funcMap);

	return geometry;
}

template<>
static Primitive::Ptr SObjLoader::Load(XMLElement * ele, Primitive::Ptr*) {
	Primitive::Ptr primitive = nullptr;

	FuncMap funcMap;
	
	Reg_Load_val<Sphere::Ptr>(funcMap, str::Sphere::type, primitive);
	Reg_Load_val<Plane::Ptr>(funcMap, str::Plane::type, primitive);
	Reg_Load_val<TriMesh::Ptr>(funcMap, str::TriMesh::type, primitive);

	LoadNode(ele, funcMap);

	return primitive;
}

template<>
static Sphere::Ptr SObjLoader::Load(XMLElement * ele, Sphere::Ptr *) {
	return ToPtr(new Sphere);
}

template<>
static Plane::Ptr SObjLoader::Load(XMLElement * ele, Plane::Ptr*) {
	return ToPtr(new Plane);
}

template<>
static TriMesh::Ptr SObjLoader::Load(XMLElement * ele, TriMesh::Ptr*) {
	TriMesh::Ptr triMesh;
	FuncMap funcMap;
	funcMap[str::TriMesh::ENUM_TYPE::INVALID] = [&](XMLElement * ele) {
		triMesh = nullptr;
	};
	funcMap[str::TriMesh::ENUM_TYPE::CODE] = [&](XMLElement * ele) {
		triMesh = nullptr;//not supprt now
	};
	funcMap[str::TriMesh::ENUM_TYPE::CUBE] = [&](XMLElement * ele) {
		triMesh = TriMesh::GenCube();
	};
	funcMap[str::TriMesh::ENUM_TYPE::PLANE] = [&](XMLElement * ele) {
		triMesh = TriMesh::GenPlane();
	};
	funcMap[str::TriMesh::ENUM_TYPE::SPHERE] = [&](XMLElement * ele) {
		triMesh = TriMesh::GenSphere();
	};
	funcMap[str::TriMesh::ENUM_TYPE::FILE] = [&](XMLElement * ele) {
		triMesh = nullptr;//not supprt now
	};

	funcMap[str::TriMesh::ENUM_TYPE::INVALID] = [&](XMLElement * ele) {};

	LoadNode(ele, funcMap);

	return triMesh;
}

// ------------ Light ----------------

template<>
static CmptLight::Ptr SObjLoader::Load(XMLElement * ele, CmptLight::Ptr*){
	auto cmpt = ToPtr(new CmptLight(nullptr, nullptr));

	FuncMap funcMap;
	Reg_Load_val<Light::Ptr>(funcMap, str::CmptLight::light, cmpt->light);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Light::Ptr SObjLoader::Load(XMLElement * ele, Light::Ptr*) {
	Light::Ptr light = nullptr;

	FuncMap funcMap;

	Reg_Load_val<AreaLight::Ptr>(funcMap, str::AreaLight::type, light);
	Reg_Load_val<PointLight::Ptr>(funcMap, str::PointLight::type, light);

	LoadNode(ele, funcMap);

	return light;
}

template<>
static AreaLight::Ptr SObjLoader::Load(XMLElement * ele, AreaLight::Ptr*) {
	auto areaLight = ToPtr(new AreaLight);

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::AreaLight::color, areaLight->color);
	Reg_Text_val(funcMap, str::AreaLight::intensity, areaLight->intensity);
	Reg_Text_val(funcMap, str::AreaLight::height, areaLight->height);
	Reg_Text_val(funcMap, str::AreaLight::width, areaLight->width);

	LoadNode(ele, funcMap);

	return areaLight;
}

template<>
static PointLight::Ptr SObjLoader::Load(XMLElement * ele, PointLight::Ptr*) {
	auto pointLight = ToPtr(new PointLight);

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::PointLight::color, pointLight->color);
	Reg_Text_val(funcMap, str::PointLight::intensity, pointLight->intensity);
	Reg_Text_val(funcMap, str::PointLight::linear, pointLight->linear);
	Reg_Text_val(funcMap, str::PointLight::quadratic, pointLight->quadratic);

	LoadNode(ele, funcMap);

	return pointLight;
}

// ------------ Material ----------------

template<>
static CmptMaterial::Ptr SObjLoader::Load(XMLElement * ele, CmptMaterial::Ptr*){
	auto cmpt = ToPtr(new CmptMaterial(nullptr, nullptr));

	FuncMap funcMap;
	Reg_Load_val<Material::Ptr>(funcMap, str::CmptMaterial::material, cmpt->material);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Material::Ptr SObjLoader::Load(XMLElement * ele, Material::Ptr*) {
	Material::Ptr material = nullptr;

	FuncMap funcMap;

	Reg_Load_val<BSDF_CookTorrance::Ptr>(funcMap, str::BSDF_CookTorrance::type, material);
	Reg_Load_val<BSDF_Diffuse::Ptr>(funcMap, str::BSDF_Diffuse::type, material);
	Reg_Load_val<BSDF_Emission::Ptr>(funcMap, str::BSDF_Emission::type, material);
	Reg_Load_val<BSDF_Glass::Ptr>(funcMap, str::BSDF_Glass::type, material);
	Reg_Load_val<BSDF_MetalWorkflow::Ptr>(funcMap, str::BSDF_MetalWorkflow::type, material);
	Reg_Load_val<BSDF_Mirror::Ptr>(funcMap, str::BSDF_Mirror::type, material);
	Reg_Load_val<BSDF_FrostedGlass::Ptr>(funcMap, str::BSDF_FrostedGlass::type, material);

	LoadNode(ele, funcMap);

	return material;
}

template<>
static BSDF_CookTorrance::Ptr SObjLoader::Load(XMLElement * ele, BSDF_CookTorrance::Ptr*) {
	auto bsdf = ToPtr(new BSDF_CookTorrance(1.5f, 0.2f));

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_CookTorrance::albedo, bsdf->albedo);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::ior, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::refletance, bsdf->refletance);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::roughness, bsdf->m);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Diffuse::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Diffuse::Ptr*){
	auto bsdf = ToPtr(new BSDF_Diffuse);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Diffuse::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_Diffuse::albedoTexture, bsdf->albedoTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Emission::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Emission::Ptr*) {
	auto bsdf = ToPtr(new BSDF_Emission);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Emission::color, bsdf->color);
	Reg_Text_val(funcMap, str::BSDF_Emission::intensity, bsdf->intensity);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Glass::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Glass::Ptr*) {
	auto bsdf = ToPtr(new BSDF_Glass);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Glass::ior, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_Glass::reflectance, bsdf->reflectance);
	Reg_Text_val(funcMap, str::BSDF_Glass::transmittance, bsdf->transmittance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_MetalWorkflow::Ptr SObjLoader::Load(XMLElement * ele, BSDF_MetalWorkflow::Ptr*) {
	auto bsdf = ToPtr(new BSDF_MetalWorkflow);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_MetalWorkflow::albedoTexture, bsdf->albedoTexture);
	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_MetalWorkflow::metallicTexture, bsdf->metallicTexture);
	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_MetalWorkflow::roughnessTexture, bsdf->roughnessTexture);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_MetalWorkflow::aoTexture, bsdf->aoTexture);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_MetalWorkflow::normalTexture, bsdf->normalTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Mirror::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Mirror::Ptr*) {
	auto bsdf = ToPtr(new BSDF_Mirror);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Mirror::reflectance, bsdf->reflectance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_FrostedGlass::Ptr SObjLoader::Load(XMLElement * ele, BSDF_FrostedGlass::Ptr*) {
	auto bsdf = ToPtr(new BSDF_FrostedGlass);

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::IOR, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_FrostedGlass::colorTexture, bsdf->colorTexture);
	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::roughnessFactor, bsdf->roughnessFactor);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_FrostedGlass::roughnessTexture, bsdf->roughnessTexture);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_FrostedGlass::aoTexture, bsdf->aoTexture);
	Reg_Load_val<Image::Ptr>(funcMap, str::BSDF_FrostedGlass::normalTexture, bsdf->normalTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

// ------------ Transform ----------------

template<>
static CmptTransform::Ptr SObjLoader::Load(XMLElement * ele, CmptTransform::Ptr*){
	auto cmpt = ToPtr(new CmptTransform(nullptr));

	FuncMap funcMap;
	Reg_Text_setVal(funcMap, str::CmptTransform::Position, cmpt, &CmptTransform::SetPosition);
	Reg_Text_setVal(funcMap, str::CmptTransform::Rotation, cmpt, &CmptTransform::SetRotation);
	Reg_Text_setVal(funcMap, str::CmptTransform::Scale, cmpt, &CmptTransform::SetScale);

	LoadNode(ele, funcMap);

	return cmpt;
}
