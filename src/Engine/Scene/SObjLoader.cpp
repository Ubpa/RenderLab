#include "SObjLoader.h"

#include "SL_Common.h"

#include <UGM/transform.h>
#include <Basic/StrAPI.h>

using namespace Ubpa;

using namespace std;

#ifdef USE_TINYXML2
using namespace tinyxml2;

template<>
const float SObjLoader::To(const Key & key) {
	return static_cast<float>(atof(key.c_str()));
}

template<>
const int SObjLoader::To(const Key & key) {
	return atoi(key.c_str());
}

template<>
const string SObjLoader::To(const Key & key) {
	return key;
}

template<>
const Ptr<Image> SObjLoader::To(const Key & key) {
	return Image::New(key);
}

template<>
const vector<unsigned> SObjLoader::To(const Key& key) {
	vector<unsigned> triangles;
	auto triangleStrs = StrAPI::Spilt(key, '\n');
	for (const auto& triangleStr : triangleStrs) {
		if (triangleStr.empty())
			continue;
		auto idxStrs = StrAPI::Spilt(triangleStr, ' ');
		for (const auto& idxStr : idxStrs)
			triangles.push_back(static_cast<unsigned>(stoi(idxStr)));
	}
	return triangles;
}

template<>
const vector<pointf3> SObjLoader::To(const Key& key) {
	vector<pointf3> positions;
	auto positionStrs = StrAPI::Spilt(key, '\n');
	for (const auto& positionStr : positionStrs) {
		if (positionStr.empty())
			continue;
		auto valStrs = StrAPI::Spilt(positionStr, ' ');
		positions.emplace_back(stof(valStrs[0]), stof(valStrs[1]), stof(valStrs[2]));
	}
	return positions;
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
static Ptr<Image> SObjLoader::Load(EleP ele) {
	FuncMap funcMap;

	Ptr<Image> img = nullptr;

	Reg_Text_val(funcMap, str::Image::path, img);

	LoadNode(ele, funcMap);

	return img;
}
// ------------ SObj ----------------

template<>
static Ptr<SObj> SObjLoader::Load(XMLElement * ele) {
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
#endif

Ptr<SObj> SObjLoader::Load(const string & path) {
#ifdef USE_TINYXML2
	XMLDocument doc;

	if (doc.LoadFile(path.c_str()) != XML_SUCCESS)
		return nullptr;

	auto tmp = SObj::New();

	return Load<Ptr<SObj>>(doc.FirstChildElement(str::SObj::type));
#else
	cout << "ERROR::SObjLoader:" << endl
		<< "\t" << "no tinyxml2 because not found it when cmake, read setup.md for more details" << endl
		<< "\t" << "load fail" << endl;
	return nullptr;
#endif
}

#ifdef USE_TINYXML2
template<>
static vector<Ptr<Component>> SObjLoader::Load(XMLElement * ele) {
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
static vector<Ptr<SObj>> SObjLoader::Load(XMLElement * ele) {
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
static Ptr<CmptCamera> SObjLoader::Load(XMLElement * ele){
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
static Ptr<CmptGeometry> SObjLoader::Load(XMLElement * ele){
	auto geometry = CmptGeometry::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Primitive>>(funcMap, str::CmptGeometry::primitive, geometry->primitive);

	LoadNode(ele, funcMap);

	return geometry;
}

template<>
static Ptr<Primitive> SObjLoader::Load(XMLElement * ele) {
	Ptr<Primitive> primitive = nullptr;

	FuncMap funcMap;
	
	Reg_Load_val<Ptr<Sphere>>(funcMap, str::Sphere::type, primitive);
	Reg_Load_val<Ptr<Plane>>(funcMap, str::Plane::type, primitive);
	Reg_Load_val<Ptr<TriMesh>>(funcMap, str::TriMesh::type, primitive);
	Reg_Load_val<Ptr<Capsule>>(funcMap, str::Capsule::type, primitive);
	Reg_Load_val<Ptr<Disk>>(funcMap, str::Disk::type, primitive);

	LoadNode(ele, funcMap);

	return primitive;
}

template<>
static Ptr<Sphere> SObjLoader::Load(XMLElement * ele) {
	return Sphere::New();
}

template<>
static Ptr<Plane> SObjLoader::Load(XMLElement * ele) {
	return Plane::New();
}

template<>
static Ptr<TriMesh> SObjLoader::Load(XMLElement * ele) {
	Ptr<TriMesh> triMesh;
	FuncMap funcMap;
	funcMap[str::TriMesh::ENUM_TYPE::INVALID] = [&](XMLElement * ele) {
		triMesh = nullptr;
	};
	funcMap[str::TriMesh::ENUM_TYPE::CODE::type] = [&](XMLElement * ele) {
		FuncMap innerFuncMap;
		vector<pointf3> positions;
		vector<unsigned> indices;
		Reg_Text_val<vector<pointf3>>(innerFuncMap, str::TriMesh::ENUM_TYPE::CODE::position, positions);
		Reg_Text_val<vector<unsigned>>(innerFuncMap, str::TriMesh::ENUM_TYPE::CODE::triangle, indices);
		LoadNode(ele, innerFuncMap);
		triMesh = TriMesh::New(indices, positions);
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
	funcMap[str::TriMesh::ENUM_TYPE::DISK] = [&](XMLElement * ele) {
		triMesh = TriMesh::GenDisk();
	};

	funcMap[str::TriMesh::ENUM_TYPE::INVALID] = [&](XMLElement * ele) {};

	LoadNode(ele, funcMap);

	return triMesh;
}

template<>
static Ptr<Capsule> SObjLoader::Load(XMLElement * ele) {
	auto capsule = Capsule::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::Capsule::height, capsule->height);

	LoadNode(ele, funcMap);

	return capsule;
}

template<>
static Ptr<Disk> SObjLoader::Load(XMLElement * ele) {
	return Disk::New();
}

// ------------ Light ----------------

template<>
static Ptr<CmptLight> SObjLoader::Load(XMLElement * ele){
	auto cmpt = CmptLight::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Light>>(funcMap, str::CmptLight::light, cmpt->light);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Ptr<Light> SObjLoader::Load(XMLElement * ele) {
	Ptr<Light> light = nullptr;

	FuncMap funcMap;

	Reg_Load_val<Ptr<AreaLight>>(funcMap, str::AreaLight::type, light);
	Reg_Load_val<Ptr<PointLight>>(funcMap, str::PointLight::type, light);
	Reg_Load_val<Ptr<DirectionalLight>>(funcMap, str::DirectionalLight::type, light);
	Reg_Load_val<Ptr<SpotLight>>(funcMap, str::SpotLight::type, light);
	Reg_Load_val<Ptr<InfiniteAreaLight>>(funcMap, str::InfiniteAreaLight::type, light);
	Reg_Load_val<Ptr<SphereLight>>(funcMap, str::SphereLight::type, light);
	Reg_Load_val<Ptr<DiskLight>>(funcMap, str::DiskLight::type, light);
	Reg_Load_val<Ptr<CapsuleLight>>(funcMap, str::CapsuleLight::type, light);

	LoadNode(ele, funcMap);

	return light;
}

template<>
static Ptr<AreaLight> SObjLoader::Load(XMLElement * ele) {
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
static Ptr<PointLight> SObjLoader::Load(XMLElement * ele) {
	auto pointLight = PointLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::PointLight::color, pointLight->color);
	Reg_Text_val(funcMap, str::PointLight::intensity, pointLight->intensity);
	Reg_Text_val(funcMap, str::PointLight::radius, pointLight->radius);

	LoadNode(ele, funcMap);

	return pointLight;
}

template<>
static Ptr<DirectionalLight> SObjLoader::Load(XMLElement * ele) {
	auto directionalLight = DirectionalLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::DirectionalLight::color, directionalLight->color);
	Reg_Text_val(funcMap, str::DirectionalLight::intensity, directionalLight->intensity);

	LoadNode(ele, funcMap);

	return directionalLight;
}

template<>
static Ptr<SpotLight> SObjLoader::Load(XMLElement * ele) {
	auto spotLight = SpotLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::SpotLight::color, spotLight->color);
	Reg_Text_val(funcMap, str::SpotLight::intensity, spotLight->intensity);
	Reg_Text_val(funcMap, str::SpotLight::radius, spotLight->radius);
	Reg_Text_val(funcMap, str::SpotLight::angle, spotLight->angle);
	Reg_Text_val(funcMap, str::SpotLight::fullRatio, spotLight->fullRatio);

	LoadNode(ele, funcMap);

	return spotLight;
}

template<>
static Ptr<InfiniteAreaLight> SObjLoader::Load(XMLElement * ele) {
	auto infiniteAreaLight = InfiniteAreaLight::New(nullptr);

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::InfiniteAreaLight::colorFactor, infiniteAreaLight->colorFactor);
	Reg_Text_val(funcMap, str::InfiniteAreaLight::intensity, infiniteAreaLight->intensity);
	Reg_Load_setVal<Image>(funcMap, str::InfiniteAreaLight::img, infiniteAreaLight, &InfiniteAreaLight::SetImg);

	LoadNode(ele, funcMap);

	return infiniteAreaLight;
}

template<>
static Ptr<SphereLight> SObjLoader::Load(XMLElement * ele) {
	auto sphereLight = SphereLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::SphereLight::color, sphereLight->color);
	Reg_Text_val(funcMap, str::SphereLight::intensity, sphereLight->intensity);
	Reg_Text_val(funcMap, str::SphereLight::radius, sphereLight->radius);

	LoadNode(ele, funcMap);

	return sphereLight;
}

template<>
static Ptr<DiskLight> SObjLoader::Load(XMLElement * ele) {
	auto diskLight = DiskLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::SphereLight::color, diskLight->color);
	Reg_Text_val(funcMap, str::SphereLight::intensity, diskLight->intensity);
	Reg_Text_val(funcMap, str::SphereLight::radius, diskLight->radius);

	LoadNode(ele, funcMap);

	return diskLight;
}

template<>
static Ptr<CapsuleLight> SObjLoader::Load(XMLElement * ele) {
	auto capsuleLight = CapsuleLight::New();

	FuncMap funcMap;
	Reg_Text_val(funcMap, str::CapsuleLight::color, capsuleLight->color);
	Reg_Text_val(funcMap, str::CapsuleLight::intensity, capsuleLight->intensity);
	Reg_Text_val(funcMap, str::CapsuleLight::radius, capsuleLight->radius);
	Reg_Text_val(funcMap, str::CapsuleLight::height, capsuleLight->height);

	LoadNode(ele, funcMap);

	return capsuleLight;
}

// ------------ Material ----------------

template<>
static Ptr<CmptMaterial> SObjLoader::Load(XMLElement * ele){
	auto cmpt = CmptMaterial::New(nullptr, nullptr);

	FuncMap funcMap;
	Reg_Load_val<Ptr<Material>>(funcMap, str::CmptMaterial::material, cmpt->material);

	LoadNode(ele, funcMap);

	return cmpt;
}

template<>
static Ptr<Material> SObjLoader::Load(XMLElement * ele) {
	Ptr<Material> material = nullptr;

	FuncMap funcMap;

	Reg_Load_val<Ptr<BSDF_CookTorrance>>(funcMap, str::BSDF_CookTorrance::type, material);
	Reg_Load_val<Ptr<BSDF_Diffuse>>(funcMap, str::BSDF_Diffuse::type, material);
	Reg_Load_val<Ptr<BSDF_Emission>>(funcMap, str::BSDF_Emission::type, material);
	Reg_Load_val<Ptr<BSDF_Glass>>(funcMap, str::BSDF_Glass::type, material);
	Reg_Load_val<Ptr<BSDF_MetalWorkflow>>(funcMap, str::BSDF_MetalWorkflow::type, material);
	Reg_Load_val<Ptr<BSDF_Mirror>>(funcMap, str::BSDF_Mirror::type, material);
	Reg_Load_val<Ptr<BSDF_FrostedGlass>>(funcMap, str::BSDF_FrostedGlass::type, material);
	Reg_Load_val<Ptr<Gooch>>(funcMap, str::Gooch::type, material);
	Reg_Load_val<Ptr<BSDF_Frostbite>>(funcMap, str::BSDF_Frostbite::type, material);

	LoadNode(ele, funcMap);

	return material;
}

template<>
static Ptr<BSDF_CookTorrance> SObjLoader::Load(XMLElement * ele) {
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
static Ptr<BSDF_Diffuse> SObjLoader::Load(XMLElement * ele){
	auto bsdf = BSDF_Diffuse::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Diffuse::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Diffuse::albedoTexture, bsdf->albedoTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Emission> SObjLoader::Load(XMLElement * ele) {
	auto bsdf = BSDF_Emission::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Emission::color, bsdf->color);
	Reg_Text_val(funcMap, str::BSDF_Emission::intensity, bsdf->intensity);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_Glass> SObjLoader::Load(XMLElement * ele) {
	auto bsdf = BSDF_Glass::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Glass::ior, bsdf->ior);
	Reg_Text_val(funcMap, str::BSDF_Glass::reflectance, bsdf->reflectance);
	Reg_Text_val(funcMap, str::BSDF_Glass::transmittance, bsdf->transmittance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_MetalWorkflow> SObjLoader::Load(XMLElement * ele) {
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
static Ptr<BSDF_Mirror> SObjLoader::Load(XMLElement * ele) {
	auto bsdf = BSDF_Mirror::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Mirror::reflectance, bsdf->reflectance);

	LoadNode(ele, funcMap);

	return bsdf;
}

template<>
static Ptr<BSDF_FrostedGlass> SObjLoader::Load(XMLElement * ele) {
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

template<>
static Ptr<Gooch> SObjLoader::Load(XMLElement * ele) {
	auto gooch = Gooch::New();

	FuncMap funcMap;
	
	Reg_Text_val(funcMap, str::Gooch::colorFactor, gooch->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::Gooch::colorTexture, gooch->colorTexture);

	LoadNode(ele, funcMap);

	return gooch;
}

template<>
static Ptr<BSDF_Frostbite> SObjLoader::Load(XMLElement * ele) {
	auto bsdf = BSDF_Frostbite::New();

	FuncMap funcMap;

	Reg_Text_val(funcMap, str::BSDF_Frostbite::colorFactor, bsdf->colorFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Frostbite::albedoTexture, bsdf->albedoTexture);
	Reg_Text_val(funcMap, str::BSDF_Frostbite::metallicFactor, bsdf->metallicFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Frostbite::metallicTexture, bsdf->metallicTexture);
	Reg_Text_val(funcMap, str::BSDF_Frostbite::roughnessFactor, bsdf->roughnessFactor);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Frostbite::roughnessTexture, bsdf->roughnessTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Frostbite::aoTexture, bsdf->aoTexture);
	Reg_Load_val<Ptr<Image>>(funcMap, str::BSDF_Frostbite::normalTexture, bsdf->normalTexture);

	LoadNode(ele, funcMap);

	return bsdf;
}

// ------------ transformf ----------------

template<>
static Ptr<CmptTransform> SObjLoader::Load(XMLElement * ele){
	auto cmpt = CmptTransform::New(nullptr);

	FuncMap funcMap;
	Reg_Text_setVal(funcMap, str::CmptTransform::Position, cmpt, &CmptTransform::SetPosition);
	void (CmptTransform::*func)(const quatf &) = &CmptTransform::SetRotation;
	Reg_Text_setVal(funcMap, str::CmptTransform::Rotation, cmpt, func);
	Reg_Text_setVal(funcMap, str::CmptTransform::Scale, cmpt, &CmptTransform::SetScale);

	LoadNode(ele, funcMap);

	return cmpt;
}
#endif
