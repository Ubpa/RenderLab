#include "SObjSaver.h"

#include "SL_Common.h"

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace tinyxml2;
using namespace std;

SObjSaver::SObjSaver() {
	Reg<SObj>();

	Reg<Camera>();

	Reg<Geometry>();
	Reg<Sphere>();
	Reg<Plane>();
	Reg<TriMesh>();

	Reg<Light>();
	Reg<AreaLight>();
	Reg<PointLight>();

	Reg<Material>();
	Reg<BSDF_CookTorrance>();
	Reg<BSDF_Diffuse>();
	Reg<BSDF_Emission>();
	Reg<BSDF_Glass>();
	Reg<BSDF_MetalWorkflow>();
	Reg<BSDF_Mirror>();
	Reg<BSDF_FrostedGlass>();

	Reg<Transform>();
}

void SObjSaver::Init(const string & path) {
	this->path = path;
	doc.Clear();
	parentEleStack.clear();
}

void SObjSaver::NewEle(const char * name, const glm::vec3 & vec) {
	NewEle(name, to_string(vec[0]) + " " + to_string(vec[1]) + " " + to_string(vec[2]));
}

void SObjSaver::Member(XMLElement * parent, const function<void()> & func) {
	parentEleStack.push_back(parent);
	func();
	parentEleStack.pop_back();
}

void SObjSaver::NewEle(const char * const name, Image::CPtr img) {
	Member(NewEle(name), [=]() {
		Visit(img);
	});
}

void SObjSaver::Visit(Image::CPtr img) {
	if (!img || !img->IsValid() || img->GetPath().empty())
		return;

	NewEle(str::Image::path, img->GetPath());
}

void SObjSaver::Visit(Image::Ptr img) {
	Visit(Image::CPtr(img));
}

void SObjSaver::Visit(SObj::Ptr sobj) {
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
				component->Accept(This());
		});

		// children
		NewEle(str::SObj::children, [=]() {
			for (auto child : sobj->GetChildren())
				child->Accept(This());
		});
	});

	// save
	if (parentEleStack.empty())
		doc.SaveFile(path.c_str());
}

void SObjSaver::Visit(Camera::Ptr camera) {
	NewEle(str::Camera::type, [=]() {
		NewEle(str::Camera::fov, camera->GetFOV());
		NewEle(str::Camera::ar, camera->GetAspectRatio());
		NewEle(str::Camera::nearPlane, camera->nearPlane);
		NewEle(str::Camera::farPlane, camera->farPlane);
	});
}

void SObjSaver::Visit(Geometry::Ptr geometry) {
	NewEle(str::Geometry::type, [=]() {
		NewEle(str::Geometry::primitive, [=]() {
			if (geometry->GetPrimitive())
				geometry->GetPrimitive()->Accept(This());
		});
	});
}

void SObjSaver::Visit(Sphere::Ptr sphere) {
	NewEle(str::Sphere::type, [=]() {
		NewEle(str::Sphere::center, sphere->center);
		NewEle(str::Sphere::radius, sphere->r);
	});
}

void SObjSaver::Visit(Plane::Ptr plane) {
	NewEle(str::Plane::type);
}

void SObjSaver::Visit(TriMesh::Ptr mesh) {
	NewEle(str::TriMesh::type, [=]() {
		if (mesh->GetType() == TriMesh::ENUM_TYPE::INVALID)
			return;

		map<TriMesh::ENUM_TYPE, function<void()>> type2func;
		type2func[TriMesh::ENUM_TYPE::INVALID] = [=]() {
			// do nothing
		};
		type2func[TriMesh::ENUM_TYPE::CODE] = [=]() {
			// not support
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
		type2func[TriMesh::ENUM_TYPE::FILE] = [=]() {
			// not support
		};

		auto target = type2func.find(mesh->GetType());
		if (target == type2func.end())
			return;

		target->second();
	});
}

void SObjSaver::Visit(Light::Ptr light) {
	NewEle(str::Light::type, [=]() {
		NewEle(str::Light::lightBase, [=]() {
			if (light->GetLight())
				light->GetLight()->Accept(This());
		});
	});
}

void SObjSaver::Visit(AreaLight::Ptr areaLight) {
	NewEle(str::AreaLight::type, [=]() {
		NewEle(str::AreaLight::color, areaLight->color);
		NewEle(str::AreaLight::intensity, areaLight->intensity);
		NewEle(str::AreaLight::width, areaLight->width);
		NewEle(str::AreaLight::height, areaLight->height);
	});
}

void SObjSaver::Visit(PointLight::Ptr pointLight) {
	NewEle(str::PointLight::type, [=]() {
		NewEle(str::PointLight::color, pointLight->color);
		NewEle(str::PointLight::intensity, pointLight->intensity);
		NewEle(str::PointLight::linear, pointLight->linear);
		NewEle(str::PointLight::quadratic, pointLight->quadratic);
	});
}

void SObjSaver::Visit(Material::Ptr material) {
	NewEle(str::Material::type, [=]() {
		NewEle(str::Material::materialBase, [=]() {
			if (material->GetMat())
				material->GetMat()->Accept(This());
		});
	});
}

void SObjSaver::Visit(BSDF_CookTorrance::Ptr bsdf){
	NewEle(str::BSDF_CookTorrance::type, [=]() {
		NewEle(str::BSDF_CookTorrance::ior, bsdf->ior);
		NewEle(str::BSDF_CookTorrance::roughness, bsdf->m);
		NewEle(str::BSDF_CookTorrance::refletance, bsdf->refletance);
		NewEle(str::BSDF_CookTorrance::albedo, bsdf->albedo);
	});
}

void SObjSaver::Visit(BSDF_Diffuse::Ptr bsdf){
	NewEle(str::BSDF_Diffuse::type, [=]() {
		NewEle(str::BSDF_Diffuse::albedoColor, bsdf->albedoColor);
		NewEle(str::BSDF_Diffuse::albedoTexture, [=]() {
			Visit(bsdf->albedoTexture);
		});
	});
}

void SObjSaver::Visit(BSDF_Emission::Ptr bsdf){
	NewEle(str::BSDF_Emission::type, [=]() {
		NewEle(str::BSDF_Emission::color, bsdf->color);
		NewEle(str::BSDF_Emission::intensity, bsdf->intensity);
	});
}

void SObjSaver::Visit(BSDF_Glass::Ptr bsdf){
	NewEle(str::BSDF_Glass::type, [=]() {
		NewEle(str::BSDF_Glass::ior, bsdf->ior);
		NewEle(str::BSDF_Glass::transmittance, bsdf->transmittance);
		NewEle(str::BSDF_Glass::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::Visit(BSDF_MetalWorkflow::Ptr bsdf){
	NewEle(str::BSDF_MetalWorkflow::type, [=]() {
		NewEle(str::BSDF_MetalWorkflow::albedoColor, bsdf->albedoColor);
		NewEle(str::BSDF_MetalWorkflow::albedoTexture, [=]() {
			Visit(bsdf->GetAlbedoTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::metallicFactor, bsdf->metallicFactor);
		NewEle(str::BSDF_MetalWorkflow::metallicTexture, [=]() {
			Visit(bsdf->GetMetallicTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::roughnessFactor, bsdf->roughnessFactor);
		NewEle(str::BSDF_MetalWorkflow::roughnessTexture, [=]() {
			Visit(bsdf->GetRoughnessTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::aoTexture, [=]() {
			Visit(bsdf->GetAOTexture());
		});

		NewEle(str::BSDF_MetalWorkflow::normalTexture, [=]() {
			Visit(bsdf->GetNormalTexture());
		});
	});
}

void SObjSaver::Visit(BSDF_FrostedGlass::Ptr bsdf) {
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

void SObjSaver::Visit(BSDF_Mirror::Ptr bsdf){
	NewEle(str::BSDF_Mirror::type, [=]() {
		NewEle(str::BSDF_Mirror::reflectance, bsdf->reflectance);
	});
}

void SObjSaver::Visit(Transform::Ptr transform) {
	NewEle(str::Transform::type, [=]() {
		NewEle(str::Transform::Position, transform->GetPosition());
		NewEle(str::Transform::Rotation, transform->GetEulerRoatation());
		NewEle(str::Transform::Scale, transform->GetScale());
	});
}
