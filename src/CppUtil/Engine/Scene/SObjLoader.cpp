#include "SObjLoader.h"

#include "SL_Common.h"

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace tinyxml2;
using namespace glm;
using namespace std;

// ------------ To ----------------

template<>
float SObjLoader::To(const std::string & key, float *) {
	return static_cast<float>(atof(key.c_str()));
}

template<>
int SObjLoader::To(const std::string & key, int *) {
	return atoi(key.c_str());
}

template<>
string SObjLoader::To(const std::string & key, string *) {
	return key;
}

template<>
vec3 SObjLoader::To(const std::string & key, vec3 *) {
	auto idx0 = key.find(' ');
	auto idx1 = key.find(' ', idx0 + 1);

	auto key0 = key.substr(0, idx0);
	auto key1 = key.substr(idx0, idx1);
	auto key2 = key.substr(idx1, key.size());

	return vec3(To<float>(key0), To<float>(key1), To<float>(key2));
}

// ------------ Basic ----------------

void SObjLoader::LoadChildrenEles(XMLElement * ele, const FuncMap & funcMap) {
	for (auto child = ele->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
		auto target = funcMap.find(child->Name());
		if (target != funcMap.cend())
			target->second(child);
	}
}

template<>
static Image::Ptr SObjLoader::Load(XMLElement * ele, Image*) {
	FuncMap funcMap;

	auto img = ToPtr(new Image);

	Reg(funcMap, str::Image::path, [=](XMLElement * ele) {
		img->Load(ele->GetText());
	});

	LoadChildrenEles(ele, funcMap);

	return img;
}

// ------------ SObj ----------------

template<>
static SObj::Ptr SObjLoader::Load(XMLElement * ele, SObj*) {
	if (ele == nullptr)
		return nullptr;

	auto sobj = ToPtr(new SObj(nullptr));

	FuncMap funcMap;

	Reg(funcMap, str::SObj::name, sobj->name);
	Reg<vector<Component::Ptr>>(funcMap, str::SObj::components, sobj);
	Reg<vector<SObj::Ptr>>(funcMap, str::SObj::children, sobj);

	LoadChildrenEles(ele, funcMap);

	return sobj;
}

SObj::Ptr SObjLoader::Load(const string & path) {
	XMLDocument doc;

	if (doc.LoadFile(path.c_str()) != XML_SUCCESS)
		return nullptr;

	auto tmp = ToPtr(new SObj(nullptr));

	return Load<SObj>(doc.FirstChildElement(str::SObj::type));
}

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, vector<Component::Ptr>*) {
	FuncMap funcMap;

	Reg<Camera>(funcMap, str::Camera::type, sobj);
	Reg<Geometry>(funcMap, str::Geometry::type, sobj);
	Reg<Light>(funcMap, str::Light::type, sobj);
	Reg<Material>(funcMap, str::Material::type, sobj);
	Reg<Transform>(funcMap, str::Transform::type, sobj);

	LoadChildrenEles(ele, funcMap);
}

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, vector<SObj::Ptr>*) {
	auto pack = GenPack(sobj, &SObj::AddChild);

	Reg<SObj>(pack, str::SObj::type);

	LoadChildrenEles(ele, pack.funcMap);
}

// ------------ Camera ----------------

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, Camera*){
	auto camera = ToPtr(new Camera(sobj));

	FuncMap funcMap;
	Reg(funcMap, str::Camera::fov, camera, &Camera::SetFOV);
	void (Camera::*f)(float) = &Camera::SetAspectRatio;
	Reg(funcMap, str::Camera::ar, camera, f);
	Reg(funcMap, str::Camera::nearClipping, camera->nearClipping);
	Reg(funcMap, str::Camera::farClipping, camera->farClipping);

	LoadChildrenEles(ele, funcMap);
}

// ------------ Geometry ----------------

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, Geometry*){
	auto geometry = ToPtr(new Geometry(sobj, nullptr));

	FuncMap funcMap;
	Reg<Primitive>(funcMap, str::Geometry::primitive, geometry);

	LoadChildrenEles(ele, funcMap);
}

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, Geometry::Ptr geometry, Primitive*) {
	auto pack = GenPack(geometry, &Geometry::SetPrimitive);

	Reg<Sphere>(pack, str::Sphere::type);
	Reg<Plane>(pack, str::Plane::type);

	LoadChildrenEles(ele, pack.funcMap);
}

template<>
static Sphere::Ptr SObjLoader::Load(XMLElement * ele, Sphere*) {
	auto sphere = ToPtr(new Sphere);

	FuncMap funcMap;
	Reg(funcMap, str::Sphere::center, sphere->center);
	Reg(funcMap, str::Sphere::radius, sphere->r);

	LoadChildrenEles(ele, funcMap);

	return sphere;
}

template<>
static Plane::Ptr SObjLoader::Load(XMLElement * ele, Plane*) {
	auto plane = ToPtr(new Plane);
	return plane;
}

// ------------ Light ----------------

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, Light*){
	auto light = ToPtr(new Light(sobj, nullptr));

	FuncMap funcMap;
	Reg<LightBase>(funcMap, str::Light::lightBase, light);

	LoadChildrenEles(ele, funcMap);
}

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, Light::Ptr light, LightBase*) {
	auto pack = GenPack(light, &Light::SetLight);

	Reg<AreaLight>(pack, str::AreaLight::type);
	Reg<PointLight>(pack, str::PointLight::type);

	LoadChildrenEles(ele, pack.funcMap);
}

template<>
static AreaLight::Ptr SObjLoader::Load(XMLElement * ele, AreaLight*) {
	auto areaLight = ToPtr(new AreaLight);

	FuncMap funcMap;
	Reg(funcMap, str::AreaLight::color, areaLight->color);
	Reg(funcMap, str::AreaLight::intensity, areaLight->intensity);
	Reg(funcMap, str::AreaLight::height, areaLight->height);
	Reg(funcMap, str::AreaLight::width, areaLight->width);

	LoadChildrenEles(ele, funcMap);

	return areaLight;
}

template<>
static PointLight::Ptr SObjLoader::Load(XMLElement * ele, PointLight*) {
	auto pointLight = ToPtr(new PointLight);

	FuncMap funcMap;
	Reg(funcMap, str::PointLight::color, pointLight->color);
	Reg(funcMap, str::PointLight::intensity, pointLight->intensity);
	Reg(funcMap, str::PointLight::linear, pointLight->linear);
	Reg(funcMap, str::PointLight::quadratic, pointLight->quadratic);

	LoadChildrenEles(ele, funcMap);

	return pointLight;
}

// ------------ Material ----------------

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, Material*){
	auto material = ToPtr(new Material(sobj, nullptr));

	FuncMap funcMap;
	Reg<MaterialBase>(funcMap, str::Material::materialBase, material);

	LoadChildrenEles(ele, funcMap);
}

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, Material::Ptr material, MaterialBase*) {
	auto pack = GenPack(material, &Material::SetMat);

	Reg<BSDF_CookTorrance>(pack, str::BSDF_CookTorrance::type);
	Reg<BSDF_Diffuse>(pack, str::BSDF_Diffuse::type);
	Reg<BSDF_Emission>(pack, str::BSDF_Emission::type);
	Reg<BSDF_Glass>(pack, str::BSDF_Glass::type);
	Reg<BSDF_MetalWorkflow>(pack, str::BSDF_MetalWorkflow::type);
	Reg<BSDF_Mirror>(pack, str::BSDF_Mirror::type);

	LoadChildrenEles(ele, pack.funcMap);
}

template<>
static BSDF_CookTorrance::Ptr SObjLoader::Load(XMLElement * ele, BSDF_CookTorrance*) {
	auto bsdf = ToPtr(new BSDF_CookTorrance(1.5f, 0.2f));

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_CookTorrance::albedo, bsdf->albedo);
	Reg(funcMap, str::BSDF_CookTorrance::ior, bsdf->ior);
	Reg(funcMap, str::BSDF_CookTorrance::refletance, bsdf->refletance);
	Reg(funcMap, str::BSDF_CookTorrance::roughness, bsdf->m);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Diffuse::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Diffuse*){
	auto bsdf = ToPtr(new BSDF_Diffuse);

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_Diffuse::albedoColor, bsdf->albedoColor);
	RegLoad(funcMap, str::BSDF_Diffuse::albedoTexture, bsdf->albedoTexture);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Emission::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Emission*) {
	auto bsdf = ToPtr(new BSDF_Emission);

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_Emission::color, bsdf->color);
	Reg(funcMap, str::BSDF_Emission::intensity, bsdf->intensity);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Glass::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Glass*) {
	auto bsdf = ToPtr(new BSDF_Glass);

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_Glass::ior, bsdf->ior);
	Reg(funcMap, str::BSDF_Glass::reflectance, bsdf->reflectance);
	Reg(funcMap, str::BSDF_Glass::transmittance, bsdf->transmittance);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_MetalWorkflow::Ptr SObjLoader::Load(XMLElement * ele, BSDF_MetalWorkflow*) {
	auto bsdf = ToPtr(new BSDF_MetalWorkflow);

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_MetalWorkflow::albedoColor, bsdf->albedoColor);
	Reg<Image>(funcMap, str::BSDF_MetalWorkflow::albedoTexture, bsdf, &BSDF_MetalWorkflow::SetAlbedoTexture);
	Reg(funcMap, str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
	Reg<Image>(funcMap, str::BSDF_MetalWorkflow::metallicTexture, bsdf, &BSDF_MetalWorkflow::SetMetallicTexture);
	Reg(funcMap, str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
	Reg<Image>(funcMap, str::BSDF_MetalWorkflow::roughnessTexture, bsdf, &BSDF_MetalWorkflow::SetRoughnessTexture);
	Reg<Image>(funcMap, str::BSDF_MetalWorkflow::aoTexture, bsdf, &BSDF_MetalWorkflow::SetAOTexture);
	Reg<Image>(funcMap, str::BSDF_MetalWorkflow::normalTexture, bsdf, &BSDF_MetalWorkflow::SetNormalTexture);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

template<>
static BSDF_Mirror::Ptr SObjLoader::Load(XMLElement * ele, BSDF_Mirror*) {
	auto bsdf = ToPtr(new BSDF_Mirror);

	FuncMap funcMap;

	Reg(funcMap, str::BSDF_Mirror::reflectance, bsdf->reflectance);

	LoadChildrenEles(ele, funcMap);

	return bsdf;
}

// ------------ Transform ----------------

template<>
static void SObjLoader::LoadAndBind(XMLElement * ele, SObj::Ptr sobj, Transform*){
	auto transform = ToPtr(new Transform(sobj));

	FuncMap funcMap;
	Reg(funcMap, str::Transform::Position, transform, &Transform::SetPosition);
	Reg<const vec3 &>(funcMap, str::Transform::Rotation, transform, &Transform::SetRotation);
	Reg(funcMap, str::Transform::Scale, transform, &Transform::SetScale);

	LoadChildrenEles(ele, funcMap);
}
