#include "SObjLoader.h"

#include "SL_Common.h"

#include <CppUtil/Basic/UGM/Transform.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace tinyxml2;
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
const Ptr<Image> SObjLoader::To(const Key & key, Ptr<Image> *) {
	return Image::New(key);
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
static Ptr<Image> SObjLoader::Load(EleP ele, Ptr<Image>*) {
	FuncMap funcMap;

	Ptr<Image> img = nullptr;

	Reg_Text_val(funcMap, str::Image::path, img);

	LoadNode(ele, funcMap);

	return img;
}

// ------------ SObj ----------------

template<>
static Ptr<SObj> SObjLoader::Load(XMLElement * ele, Ptr<SObj>*) {
	if (ele == nullptr)
		return nullptr;

	auto sobj = SObj::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::SObj::name, sobj->name);
	Reg_Load_Lambda(funcMap, str::SObj::components, [&](const vector<Ptr<Component>> & components) {
		for (const auto cmpt : components)
			sobj->AttachComponent(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::SObj::children, [&](const vector<Ptr<SObj>> & children) {
		for (const auto child : children)
			sobj->AddChild(child);
	});

	LoadNode(ele, funcMap);

	return sobj;
}

Ptr<SObj> SObjLoader::Load(const string & path) {
	XMLDocument doc;

	if (doc.LoadFile(path.c_str()) != XML_SUCCESS)
		return nullptr;

	auto tmp = SObj::New();

	return Load<Ptr<SObj>>(doc.FirstChildElement(str::SObj::type));
}

template<>
static vector<Ptr<Component>> SObjLoader::Load(XMLElement * ele, vector<Ptr<Component>>*) {
	FuncMap funcMap;

	vector<Ptr<Component>> cmpts;

	Reg_Load_Lambda(funcMap, str::CmptCamera::type, [&](const Ptr<CmptCamera> & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptGeometry::type, [&](const Ptr<CmptGeometry> & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptLight::type, [&](const Ptr<CmptLight> & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptMaterial::type, [&](const Ptr<CmptMaterial> & cmpt) {
		cmpts.push_back(cmpt);
	});
	Reg_Load_Lambda(funcMap, str::CmptTransform::type, [&](const Ptr<CmptTransform> & cmpt) {
		cmpts.push_back(cmpt);
	});

	LoadNode(ele, funcMap);

	return cmpts;
}

template<>
static vector<Ptr<SObj>> SObjLoader::Load(XMLElement * ele, vector<Ptr<SObj>>*) {
	vector<Ptr<SObj>> children;

	FuncMap funcMap;

	Reg_Load_Lambda(funcMap, str::SObj::type, [&](const Ptr<SObj> & child) {
		children.push_back(child);
	});

	LoadNode(ele, funcMap);

	return children;
}

// ------------ Camera ----------------

template<>
static Ptr<CmptCamera> SObjLoader::Load(XMLElement * ele, Ptr<CmptCamera>*){
	auto cmpt = CmptCamera::New(nullptr);

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
static Ptr<CmptGeometry> SObjLoader::Load(XMLElement * ele, Ptr<CmptGeometry>*){
	auto geometry = CmptGeometry::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Primitive>>(funcMap, str::CmptGeometry::primitive, geometry->primitive);

	LoadNode(ele, funcMap);

	return geometry;
}

template<>
static Ptr<Primitive> SObjLoader::Load(XMLElement * ele, Ptr<Primitive>*) {
	Ptr<Primitive> primitive = nullptr;

	FuncMap funcMap;
	
	Reg_Load_val<Ptr<Sphere>>(funcMap, str::Sphere::type, primitive);
	Reg_Load_val<Ptr<Plane>>(funcMap, str::Plane::type, primitive);
	Reg_Load_val<Ptr<TriMesh>>(funcMap, str::TriMesh::type, primitive);

	LoadNode(ele, funcMap);

	return primitive;
}

template<>
static Ptr<Sphere> SObjLoader::Load(XMLElement * ele, Ptr<Sphere> *) {
	return Sphere::New();
}

template<>
static Ptr<Plane> SObjLoader::Load(XMLElement * ele, Ptr<Plane>*) {
	return Plane::New();
}

template<>
static Ptr<TriMesh> SObjLoader::Load(XMLElement * ele, Ptr<TriMesh>*) {
	Ptr<TriMesh> triMesh;
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
static Ptr<CmptLight> SObjLoader::Load(XMLElement * ele, Ptr<CmptLight>*){
	auto cmpt = CmptLight::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Light>>(funcMap, str::CmptLight::light, cmpt->light);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Ptr<Light> SObjLoader::Load(XMLElement * ele, Ptr<Light>*) {
	Ptr<Light> light = nullptr;

	FuncMap funcMap;

	Reg_Load_val<Ptr<AreaLight>>(funcMap, str::AreaLight::type, light);
	Reg_Load_val<Ptr<PointLight>>(funcMap, str::PointLight::type, light);
	Reg_Load_val<Ptr<DirectionalLight>>(funcMap, str::DirectionalLight::type, light);
	Reg_Load_val<Ptr<SpotLight>>(funcMap, str::SpotLight::type, light);

	LoadNode(ele, funcMap);

	return light;
}

template<>
static Ptr<AreaLight> SObjLoader::Load(XMLElement * ele, Ptr<AreaLight>*) {
	auto areaLight = AreaLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::AreaLight::color, areaLight->color);
	Reg_Text_val(funcMap, str::AreaLight::intensity, areaLight->intensity);
	Reg_Text_val(funcMap, str::AreaLight::height, areaLight->height);
	Reg_Text_val(funcMap, str::AreaLight::width, areaLight->width);

	LoadNode(ele, funcMap);

	return areaLight;
}

template<>
static Ptr<PointLight> SObjLoader::Load(XMLElement * ele, Ptr<PointLight>*) {
	auto pointLight = PointLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::PointLight::color, pointLight->color);
	Reg_Text_val(funcMap, str::PointLight::intensity, pointLight->intensity);
	Reg_Text_val(funcMap, str::PointLight::linear, pointLight->linear);
	Reg_Text_val(funcMap, str::PointLight::quadratic, pointLight->quadratic);

	LoadNode(ele, funcMap);

	return pointLight;
}

template<>
static Ptr<DirectionalLight> SObjLoader::Load(XMLElement * ele, Ptr<DirectionalLight>*) {
	auto directionalLight = DirectionalLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::DirectionalLight::color, directionalLight->color);
	Reg_Text_val(funcMap, str::DirectionalLight::intensity, directionalLight->intensity);

	LoadNode(ele, funcMap);

	return directionalLight;
}

template<>
static Ptr<SpotLight> SObjLoader::Load(XMLElement * ele, Ptr<SpotLight>*) {
	auto spotLight = SpotLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::SpotLight::color, spotLight->color);
	Reg_Text_val(funcMap, str::SpotLight::intensity, spotLight->intensity);
	Reg_Text_val(funcMap, str::SpotLight::linear, spotLight->linear);
	Reg_Text_val(funcMap, str::SpotLight::quadratic, spotLight->quadratic);
	Reg_Text_val(funcMap, str::SpotLight::angle, spotLight->angle);
	Reg_Text_val(funcMap, str::SpotLight::fullRatio, spotLight->fullRatio);

	LoadNode(ele, funcMap);

	return spotLight;
}

// ------------ Material ----------------

template<>
static Ptr<CmptMaterial> SObjLoader::Load(XMLElement * ele, Ptr<CmptMaterial>*){
	auto cmpt = CmptMaterial::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Material>>(funcMap, str::CmptMaterial::material, cmpt->material);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Ptr<Material> SObjLoader::Load(XMLElement * ele, Ptr<Material>*) {
	Ptr<Material> material = nullptr;

	FuncMap funcMap;

	Reg_Load_val<Ptr<BSDF_CookTorrance>>(funcMap, str::BSDF_CookTorrance::type, material);
	Reg_Load_val<Ptr<BSDF_Diffuse>>(funcMap, str::BSDF_Diffuse::type, material);
	Reg_Load_val<Ptr<BSDF_Emission>>(funcMap, str::BSDF_Emission::type, material);
	Reg_Load_val<Ptr<BSDF_Glass>>(funcMap, str::BSDF_Glass::type, material);
	Reg_Load_val<Ptr<BSDF_MetalWorkflow>>(funcMap, str::BSDF_MetalWorkflow::type, material);
	Reg_Load_val<Ptr<BSDF_Mirror>>(funcMap, str::BSDF_Mirror::type, material);
	Reg_Load_val<Ptr<BSDF_FrostedGlass>>(funcMap, str::BSDF_FrostedGlass::type, material);

	LoadNode(ele, funcMap);

	return material;
}

template<>
static Ptr<BSDF_CookTorrance> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_CookTorrance>*) {
	auto bsdf = BSDF_CookTorrance::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_CookTorrance::albedo, bsdf->albedo);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::ior, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::refletance, bsdf->refletance);
	Reg_Text_val(funcMap, str::BSDF_CookTorrance::roughness, bsdf->m);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Diffuse> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_Diffuse>*){
	auto bsdf = BSDF_Diffuse::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Diffuse::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Diffuse::albedoTexture, bsdf->albedoTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Emission> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_Emission>*) {
	auto bsdf = BSDF_Emission::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Emission::color, bsdf->color);
	Reg_Text_val(funcMap, str::BSDF_Emission::intensity, bsdf->intensity);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Glass> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_Glass>*) {
	auto bsdf = BSDF_Glass::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Glass::ior, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_Glass::reflectance, bsdf->reflectance);
	Reg_Text_val(funcMap, str::BSDF_Glass::transmittance, bsdf->transmittance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_MetalWorkflow> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_MetalWorkflow>*) {
	auto bsdf = BSDF_MetalWorkflow::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_MetalWorkflow::albedoTexture, bsdf->albedoTexture);
	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_MetalWorkflow::metallicTexture, bsdf->metallicTexture);
	Reg_Text_val(funcMap, str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_MetalWorkflow::roughnessTexture, bsdf->roughnessTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_MetalWorkflow::aoTexture, bsdf->aoTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_MetalWorkflow::normalTexture, bsdf->normalTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Mirror> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_Mirror>*) {
	auto bsdf = BSDF_Mirror::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Mirror::reflectance, bsdf->reflectance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_FrostedGlass> SObjLoader::Load(XMLElement * ele, Ptr<BSDF_FrostedGlass>*) {
	auto bsdf = BSDF_FrostedGlass::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::IOR, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_FrostedGlass::colorTexture, bsdf->colorTexture);
	Reg_Text_val(funcMap, str::BSDF_FrostedGlass::roughnessFactor, bsdf->roughnessFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_FrostedGlass::roughnessTexture, bsdf->roughnessTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_FrostedGlass::aoTexture, bsdf->aoTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_FrostedGlass::normalTexture, bsdf->normalTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

// ------------ Transform ----------------

template<>
static Ptr<CmptTransform> SObjLoader::Load(XMLElement * ele, Ptr<CmptTransform>*){
	auto cmpt = CmptTransform::New(nullptr);

	FuncMap funcMap;
	Reg_Text_setVal(funcMap, str::CmptTransform::Position, cmpt, &CmptTransform::SetPosition);
	void (CmptTransform::*func)(const Quatf &) = &CmptTransform::SetRotation;
	Reg_Text_setVal(funcMap, str::CmptTransform::Rotation, cmpt, func);
	Reg_Text_setVal(funcMap, str::CmptTransform::Scale, cmpt, &CmptTransform::SetScale);

	LoadNode(ele, funcMap);

	return cmpt;
}
