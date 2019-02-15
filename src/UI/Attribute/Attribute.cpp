#include <UI/Attribute.h>

#include <UI/Grid.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/AllComponents.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Engine/AreaLight.h>

#include <CppUtil/Basic/EleVisitor.h>
#include <CppUtil/Basic/Math.h>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;
using namespace Ui;
using namespace glm;

class Attribute::ComponentVisitor : public EleVisitor {
	ELEVISITOR_SETUP(ComponentVisitor)
public:
	ComponentVisitor(Attribute * attr)
		: attr(attr) {
		Reg<Camera>();

		Reg<Geometry>();
		Reg<Sphere>();
		Reg<Plane>();
		Reg<TriMesh>();

		Reg<Light>();
		Reg<AreaLight>();

		Reg<Material>();
		Reg<BSDF_Diffuse>();
		Reg<BSDF_Glass>();
		Reg<BSDF_Mirror>();
		Reg<BSDF_Emission>();

		Reg<Transform>();
	}

private:
	void Visit(Camera::Ptr camera);

	void Visit(Geometry::Ptr geo);
	void Visit(Sphere::Ptr sphere);
	void Visit(Plane::Ptr plane);
	void Visit(TriMesh::Ptr mesh);

	void Visit(Light::Ptr light);
	void Visit(AreaLight::Ptr light);

	void Visit(Material::Ptr material);
	void Visit(BSDF_Diffuse::Ptr bsdf);
	void Visit(BSDF_Emission::Ptr bsdf);
	void Visit(BSDF_Glass::Ptr bsdf);
	void Visit(BSDF_Mirror::Ptr bsdf);

	void Visit(Transform::Ptr transform);

private:
	QWidget * GenItem(Component::Ptr component, const QString & str) {
		auto item = new QWidget;
		attr->component2item[component] = item;
		attr->item2component[item] = component;
		attr->componentType2item[typeid(*component)] = item;
		attr->tbox->addItem(item, str);
		return item;
	}

	Grid GenGrid(QWidget * item) {
		auto vboxLayout = new QVBoxLayout(item);
		auto gridLayout = new QGridLayout();
		auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		vboxLayout->addLayout(gridLayout);
		vboxLayout->addItem(verticalSpacer);

		return Grid(item, gridLayout);
	}

private:
	Attribute * attr;
};

// -------------- Transform --------------

void Attribute::ComponentVisitor::Visit(Transform::Ptr transform) {
	auto item = GenItem(transform, "Transform");
	auto grid = GenGrid(item);

	// position
	grid.AddTitle("- Position");
	auto pos = transform->GetPosition();
	grid.AddEditVal("x", pos.x, 0.1, [transform](double x) {
		auto pos = transform->GetPosition();
		pos.x = x;
		transform->SetPosition(pos);
	});

	grid.AddEditVal("y", pos.y, 0.1, [transform](double y) {
		auto pos = transform->GetPosition();
		pos.y = y;
		transform->SetPosition(pos);
	});

	grid.AddEditVal("z", pos.z, 0.1, [transform](double z) {
		auto pos = transform->GetPosition();
		pos.z = z;
		transform->SetPosition(pos);
	});

	// rotation
	grid.AddTitle("- Rotation");
	auto rotation = degrees(transform->GetEulerRoatation());
	grid.AddEditVal("x", rotation.x, 1.0, [transform](double x) {
		auto rotation = transform->GetEulerRoatation();
		rotation.x = radians(x);
		transform->SetRotation(rotation);
	});

	grid.AddEditVal("y", rotation.y, 1.0, [transform](double y) {
		auto rotation = transform->GetEulerRoatation();
		rotation.y = radians(y);
		transform->SetRotation(rotation);
	});

	grid.AddEditVal("z", rotation.y, 1.0, [transform](double z) {
		auto rotation = transform->GetEulerRoatation();
		rotation.z = radians(z);
		transform->SetRotation(rotation);
	});

	// scale
	grid.AddTitle("- Scale");
	auto scale = transform->GetScale();
	grid.AddEditVal("x", scale.x, 0.1, [transform](double x) {
		auto scale = transform->GetScale();
		scale.x = x;
		transform->SetScale(scale);
	});

	grid.AddEditVal("y", scale.y, 0.1, [transform](double y) {
		auto scale = transform->GetScale();
		scale.y = y;
		transform->SetScale(scale);
	});

	grid.AddEditVal("z", scale.z, 0.1, [transform](double z) {
		auto scale = transform->GetScale();
		scale.z = z;
		transform->SetScale(scale);
	});

}

// -------------- Camera --------------

void Attribute::ComponentVisitor::Visit(Camera::Ptr camera) {
	auto item = GenItem(camera, "Camera");
	auto grid = GenGrid(item);
	
	grid.AddEditVal("- Field of View", camera->GetFOV(), 1, [camera](double fov) {
		camera->SetFOV(fov);
	});
}

// -------------- Geometry --------------

void Attribute::ComponentVisitor::Visit(Geometry::Ptr geo) {
	GenItem(geo, "Geometry");
	if (geo->GetPrimitive())
		geo->GetPrimitive()->Accept(This());
}

void Attribute::ComponentVisitor::Visit(Sphere::Ptr sphere) {
	auto grid = GenGrid(attr->componentType2item[typeid(Geometry)]);
	auto & center = sphere->center;
	auto & r = sphere->r;

	grid.AddTitle("[ Sphere ]");

	grid.AddTitle("- center");
	grid.AddEditVal("x", center.x, 0.1);
	grid.AddEditVal("y", center.y, 0.1);
	grid.AddEditVal("z", center.z, 0.1);

	grid.AddEditVal("- radius", r, 0.1);
}

void Attribute::ComponentVisitor::Visit(Plane::Ptr plane) {
	auto grid = GenGrid(attr->componentType2item[typeid(Geometry)]);
	grid.AddTitle("[ Plane ]");
	grid.AddTitle("empty");
}

void Attribute::ComponentVisitor::Visit(TriMesh::Ptr mesh) {
	auto grid = GenGrid(attr->componentType2item[typeid(Geometry)]);
	grid.AddTitle("[ Mesh ]");
	grid.AddShowVal("- Triangle", mesh->GetIndice().size() / 3);
	grid.AddShowVal("- Vertex", mesh->GetPositions().size());
}

// -------------- Material --------------

void Attribute::ComponentVisitor::Visit(Material::Ptr material) {
	GenItem(material, "Material");
	if (material->GetMat())
		material->GetMat()->Accept(This());
}

void Attribute::ComponentVisitor::Visit(BSDF_Diffuse::Ptr bsdf) {
	auto grid = GenGrid(attr->componentType2item[typeid(Material)]);
	grid.AddTitle("[ BSDF -- Diffuse ]");
	grid.AddEditColor("- Albedo", bsdf->albedo);
}

void Attribute::ComponentVisitor::Visit(BSDF_Emission::Ptr bsdf) {
	auto grid = GenGrid(attr->componentType2item[typeid(Material)]);
	grid.AddTitle("[ BSDF -- Emission ]");
	grid.AddEditColor("- Color", bsdf->color);
	grid.AddEditVal("- Intensity", bsdf->intensity, 0.1);
}

void Attribute::ComponentVisitor::Visit(BSDF_Glass::Ptr bsdf) {
	auto grid = GenGrid(attr->componentType2item[typeid(Material)]);
	grid.AddTitle("[ BSDF -- Glass ]");
	grid.AddEditColor("- Transmittance", bsdf->transmittance);
	grid.AddEditColor("- Reflectance", bsdf->reflectance);
	grid.AddEditVal("- ior", bsdf->ior, 0.01);
}

void Attribute::ComponentVisitor::Visit(BSDF_Mirror::Ptr bsdf) {
	auto grid = GenGrid(attr->componentType2item[typeid(Material)]);
	grid.AddTitle("[ BSDF -- Mirror ]");
	grid.AddEditColor("- Reflectance", bsdf->reflectance);
}

// -------------- Light --------------

void Attribute::ComponentVisitor::Visit(Light::Ptr light) {
	GenItem(light, "Light");
	if (light->GetLight())
		light->GetLight()->Accept(This());
}

void Attribute::ComponentVisitor::Visit(AreaLight::Ptr light) {
	auto grid = GenGrid(attr->componentType2item[typeid(Light)]);
	grid.AddTitle("[ Light -- Arealight ]");
	grid.AddEditColor("- Color", light->color);
	grid.AddEditVal("- Intensity", light->intensity, 0.1);
	grid.AddEditVal("- Width", light->width, 0.1);
	grid.AddEditVal("- Height", light->height, 0.1);
}

// -------------- Attribute --------------

Attribute::Attribute()
	: tbox(nullptr), sobj(nullptr), visitor(ToPtr(new ComponentVisitor(this))){
}

void Attribute::Init(QToolBox * tbox) {
	this->tbox = tbox;
	SetSObj(nullptr);
}

void Attribute::SetSObj(SObj::Ptr sobj) {
	if (!tbox)
		return;

	this->sobj = sobj;

	// clear
	int num = tbox->count();
	while (num-- > 0) {
		// remove item ²»»áÉ¾³ý item
		auto item = tbox->widget(0);
		tbox->removeItem(0);
		delete item;
	}
	component2item.clear();
	item2component.clear();

	if (sobj == nullptr)
		return;

	for (auto component : sobj->GetAllComponents())
		component->Accept(visitor);
}
