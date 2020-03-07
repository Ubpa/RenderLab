#include "SObjSaver.h"

#include "SL_Common.h"

using namespace Ubpa;
using namespace std;

#ifdef USE_TINYXML2
using namespace tinyxml2;
#else
#include <iostream>
#endif // USE_TINYXML2


SObjSaver::SObjSaver() {
#ifdef USE_TINYXML2
	Regist<CmptCamera,
		CmptGeometry,
		CmptLight,
		CmptMaterial,
		CmptTransform>();

	Regist<Sphere,
		Plane,
		TriMesh,
		Capsule,
		Disk>();

	Regist<AreaLight,
		PointLight,
		DirectionalLight,
		SpotLight,
		InfiniteAreaLight,
		SphereLight,
		DiskLight,
		CapsuleLight>();

	Regist<BSDF_CookTorrance,
		BSDF_Diffuse,
		BSDF_Emission,
		BSDF_Glass,
		BSDF_MetalWorkflow,
		BSDF_Mirror,
		BSDF_FrostedGlass,
		BSDF_Frostbite,
		Gooch>();
#endif
}

void SObjSaver::Init(const string & path) {
	this->path = path;
#ifdef USE_TINYXML2
	doc.Clear();
	parentEleStack.clear();
#endif
}

#ifdef USE_TINYXML2
void SObjSaver::Member(XMLElement * parent, const function<void()> & func) {
	parentEleStack.push_back(parent);
	func();
	parentEleStack.pop_back();
}

void SObjSaver::NewEle(const char * const name, PtrC<Image> img) {
	Member(NewEle(name), [=]() {
		Visit(img);
	});
}

void SObjSaver::Visit(PtrC<Image> img) {
	if (!img || !img->IsValid() || img->GetPath().empty())
		return;

	NewEle(str::Image::path, img->GetPath());
}

void SObjSaver::Visit(Ptr<Image> img) {
	Visit(PtrC<Image>(img));
}
#endif

void SObjSaver::Visit(Ptr<SObj> sobj) {
#ifdef USE_TINYXML2
	// sobj
	auto ele = doc.NewElement(str::SObj::type);
	if (parentEleStack.empty())
		doc.InsertEndChild(ele);
	else
		parentEleStack.back()->InsertEndChild(ele);

	Member(ele, [=](){
		// name
		NewEle(str::SObj::name, sobj->name);

		// components
		NewEle(str::SObj::components, [=]() {
			for (auto component : sobj->GetAllComponents())
				Visit(component);
		});

		// children
		NewEle(str::SObj::children, [=]() {
			for (auto child : sobj->GetChildren())
				Visit(child);
		});
	});

	// save
	if (parentEleStack.empty())
		doc.SaveFile(path.c_str());
#else
	cout << "ERROR::SObjSaver:" << endl
		<< "\t" << "no tinyxml2 because not found it when cmake, read setup.md for more details" << endl
		<< "\t" << "save fail" << endl;
#endif
}

#ifdef USE_TINYXML2
// ----------------- Camera -----------------

void SObjSaver::ImplVisit(Ptr<CmptCamera> cmpt) {
	NewEle(str::CmptCamera::type, [=]() {
		NewEle(str::CmptCamera::fov, cmpt->GetFOV());
		NewEle(str::CmptCamera::ar, cmpt->GetAspectRatio());
		NewEle(str::CmptCamera::nearPlane, cmpt->nearPlane);
		NewEle(str::CmptCamera::farPlane, cmpt->farPlane);
	});
}

// ----------------- transformf -----------------

void SObjSaver::ImplVisit(Ptr<CmptTransform> cmpt) {
	NewEle(str::CmptTransform::type, [=]() {
		NewEle(str::CmptTransform::Position, cmpt->GetPosition());
		NewEle(str::CmptTransform::Rotation, cmpt->GetRotation());
		NewEle(str::CmptTransform::Scale, cmpt->GetScale());
	});
}

// ----------------- Geometry -----------------

void SObjSaver::ImplVisit(Ptr<CmptGeometry> cmpt) {
	NewEle(str::CmptGeometry::type, [=]() {
		NewEle(str::CmptGeometry::primitive, [=]() {
			if (cmpt->primitive)
				Visit(cmpt->primitive);
		});
	});
}

void SObjSaver::ImplVisit(Ptr<Sphere> sphere) {
	NewEle(str::Sphere::type);
}

void SObjSaver::ImplVisit(Ptr<Plane> plane) {
	NewEle(str::Plane::type);
}

void SObjSaver::ImplVisit(Ptr<TriMesh> mesh) {
	NewEle(str::TriMesh::type, [=]() {
		if (mesh->GetType() == TriMesh::ENUM_TYPE::INVALID)
			return;

		map<TriMesh::ENUM_TYPE, function<void()>> type2func;
		type2func[TriMesh::ENUM_TYPE::INVALID] = [=]() {
			// do nothing
		};
		type2func[TriMesh::ENUM_TYPE::CODE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::CODE::type, [=]() {
				string positionsStr = "\n";
				string indicesStr = "\n";
				positionsStr.reserve(mesh->GetPositions().size() * 3 * 12);
				int digit = static_cast<int>(log10(mesh->GetPositions().size())) + 1;
				const auto& indices = mesh->GetIndice();
				indicesStr.reserve(indices.size() * (digit+1));
				for (auto pos : mesh->GetPositions())
					positionsStr += to_string(pos[0]) + " " + to_string(pos[1]) + " " + to_string(pos[2]) + "\n";
				for (size_t i = 0; i < mesh->GetIndice().size(); i += 3)
					indicesStr += to_string(indices[i]) + " " + to_string(indices[i + 1]) + " " + to_string(indices[i + 2]) + "\n";
				NewEle(str::TriMesh::ENUM_TYPE::CODE::position, positionsStr);
				NewEle(str::TriMesh::ENUM_TYPE::CODE::triangle, indicesStr);
			});
		};

		type2func[TriMesh::ENUM_TYPE::CUBE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::CUBE);
		};
		type2func[TriMesh::ENUM_TYPE::SPHERE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::SPHERE);
		};
		type2func[TriMesh::ENUM_TYPE::PLANE] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::PLANE);
		};
		type2func[TriMesh::ENUM_TYPE::DISK] = [=]() {
			NewEle(str::TriMesh::ENUM_TYPE::DISK);
		};

		auto target = type2func.find(mesh->GetType());
		if (target == type2func.end())
			return;

		target->second();
	});
}

void SObjSaver::ImplVisit(Ptr<Capsule> capsule) {
	NewEle(str::Capsule::type, [=]() {
		NewEle(str::Capsule::height, capsule->height);
	});
}

void SObjSaver::ImplVisit(Ptr<Disk> disk) {
	NewEle(str::Disk::type);
}

// ----------------- Light -----------------

void SObjSaver::ImplVisit(Ptr<CmptLight> cmpt) {
	NewEle(str::CmptLight::type, [=]() {
		NewEle(str::CmptLight::light, [=]() {
			if (cmpt->light)
				Visit(cmpt->light);
		});
	});
}

void SObjSaver::ImplVisit(Ptr<AreaLight> areaLight) {
	NewEle(str::AreaLight::type, [=]() {
		NewEle(str::AreaLight::color, areaLight->color);
		NewEle(str::AreaLight::intensity, areaLight->intensity);
		NewEle(str::AreaLight::width, areaLight->width);
		NewEle(str::AreaLight::height, areaLight->height);
	});
}

void SObjSaver::ImplVisit(Ptr<PointLight> pointLight) {
	NewEle(str::PointLight::type, [=]() {
		NewEle(str::PointLight::color, pointLight->color);
		NewEle(str::PointLight::intensity, pointLight->intensity);
		NewEle(str::PointLight::radius, pointLight->radius);
	});
}

void SObjSaver::ImplVisit(Ptr<DirectionalLight> directionalLight) {
	NewEle(str::DirectionalLight::type, [=]() {
		NewEle(str::PointLight::color, directionalLight->color);
		NewEle(str::PointLight::intensity, directionalLight->intensity);
	});
}

void SObjSaver::ImplVisit(Ptr<SpotLight> spotLight) {
	NewEle(str::SpotLight::type, [=]() {
		NewEle(str::SpotLight::color, spotLight->color);
		NewEle(str::SpotLight::intensity, spotLight->intensity);
		NewEle(str::SpotLight::radius, spotLight->radius);
		NewEle(str::SpotLight::angle, spotLight->angle);
		NewEle(str::SpotLight::fullRatio, spotLight->fullRatio);
	});
}

void SObjSaver::ImplVisit(Ptr<InfiniteAreaLight> infiniteAreaLight) {
	NewEle(str::InfiniteAreaLight::type, [=]() {
		NewEle(str::InfiniteAreaLight::colorFactor, infiniteAreaLight->colorFactor);
		NewEle(str::InfiniteAreaLight::intensity, infiniteAreaLight->intensity);
		NewEle(str::InfiniteAreaLight::img, [=]() {
			Visit(infiniteAreaLight->GetImg());
		});
	});
}

void SObjSaver::ImplVisit(Ptr<DiskLight> diskLight) {
	NewEle(str::DiskLight::type, [=]() {
		NewEle(str::DiskLight::color, diskLight->color);
		NewEle(str::DiskLight::intensity, diskLight->intensity);
		NewEle(str::DiskLight::radius, diskLight->radius);
	});
}

void SObjSaver::ImplVisit(Ptr<SphereLight> sphereLight) {
	NewEle(str::SphereLight::type, [=]() {
		NewEle(str::SphereLight::color, sphereLight->color);
		NewEle(str::SphereLight::intensity, sphereLight->intensity);
		NewEle(str::SphereLight::radius, sphereLight->radius);
	});
}

void SObjSaver::ImplVisit(Ptr<CapsuleLight> capsuleLight) {
	NewEle(str::CapsuleLight::type, [=]() {
		NewEle(str::CapsuleLight::color, capsuleLight->color);
		NewEle(str::CapsuleLight::intensity, capsuleLight->intensity);
		NewEle(str::CapsuleLight::radius, capsuleLight->radius);
		NewEle(str::CapsuleLight::height, capsuleLight->height);
	});
}

// ----------------- Material -----------------

void SObjSaver::ImplVisit(Ptr<CmptMaterial> cmpt) {
	NewEle(str::CmptMaterial::type, [=]() {
		NewEle(str::CmptMaterial::material, [=]() {
			if (cmpt->material)
				Visit(cmpt->material);
		});
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_CookTorrance> bsdf){
	NewEle(str::BSDF_CookTorrance::type, [=]() {
		NewEle(str::BSDF_CookTorrance::ior, bsdf->ior);
		NewEle(str::BSDF_CookTorrance::roughness, bsdf->m);
		NewEle(str::BSDF_CookTorrance::refletance, bsdf->refletance);
		NewEle(str::BSDF_CookTorrance::albedo, bsdf->albedo);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_Diffuse> bsdf){
	NewEle(str::BSDF_Diffuse::type, [=]() {
		NewEle(str::BSDF_Diffuse::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_Diffuse::albedoTexture, [=]() {
			Visit(bsdf->albedoTexture);
		});
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_Emission> bsdf){
	NewEle(str::BSDF_Emission::type, [=]() {
		NewEle(str::BSDF_Emission::color, bsdf->color);
		NewEle(str::BSDF_Emission::intensity, bsdf->intensity);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_Glass> bsdf){
	NewEle(str::BSDF_Glass::type, [=]() {
		NewEle(str::BSDF_Glass::ior, bsdf->ior);
		NewEle(str::BSDF_Glass::transmittance, bsdf->transmittance);
		NewEle(str::BSDF_Glass::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_MetalWorkflow> bsdf){
	NewEle(str::BSDF_MetalWorkflow::type, [=]() {
		NewEle(str::BSDF_MetalWorkflow::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_MetalWorkflow::albedoTexture, bsdf->albedoTexture);

		NewEle(str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
		NewEle(str::BSDF_MetalWorkflow::metallicTexture, bsdf->metallicTexture);

		NewEle(str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_MetalWorkflow::roughnessTexture, bsdf->roughnessTexture);

		NewEle(str::BSDF_MetalWorkflow::aoTexture, bsdf->aoTexture);

		NewEle(str::BSDF_MetalWorkflow::normalTexture, bsdf->normalTexture);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_FrostedGlass> bsdf) {
	NewEle(str::BSDF_FrostedGlass::type, [=]() {
		NewEle(str::BSDF_FrostedGlass::IOR, bsdf->ior);

		NewEle(str::BSDF_FrostedGlass::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_FrostedGlass::colorTexture, bsdf->colorTexture);

		NewEle(str::BSDF_FrostedGlass::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_FrostedGlass::roughnessTexture, bsdf->roughnessTexture);

		NewEle(str::BSDF_FrostedGlass::aoTexture, bsdf->aoTexture);

		NewEle(str::BSDF_FrostedGlass::normalTexture, bsdf->normalTexture);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_Mirror> bsdf){
	NewEle(str::BSDF_Mirror::type, [=]() {
		NewEle(str::BSDF_Mirror::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::ImplVisit(Ptr<Gooch> gooch) {
	NewEle(str::Gooch::type, [=]() {
		NewEle(str::Gooch::colorFactor, gooch->colorFactor);
		NewEle(str::Gooch::colorTexture, gooch->colorTexture);
	});
}

void SObjSaver::ImplVisit(Ptr<BSDF_Frostbite> bsdf) {
	NewEle(str::BSDF_Frostbite::type, [=]() {
		NewEle(str::BSDF_Frostbite::colorFactor, bsdf->colorFactor);
		NewEle(str::BSDF_Frostbite::albedoTexture, bsdf->albedoTexture);

		NewEle(str::BSDF_Frostbite::metallicFactor, bsdf->metallicFactor);
		NewEle(str::BSDF_Frostbite::metallicTexture, bsdf->metallicTexture);

		NewEle(str::BSDF_Frostbite::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_Frostbite::roughnessTexture, bsdf->roughnessTexture);

		NewEle(str::BSDF_Frostbite::aoTexture, bsdf->aoTexture);

		NewEle(str::BSDF_Frostbite::normalTexture, bsdf->normalTexture);
	});
}
#endif
