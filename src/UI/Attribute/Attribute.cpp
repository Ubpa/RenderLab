#include <UI/Attribute.h>

#include <UI/Grid.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/AllComponents.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/PointLight.h>

#include <CppUtil/Basic/EleVisitor.h>
#include <CppUtil/Basic/Math.h>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <qcombobox.h>

#include <tuple>

using namespace Ui;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;
using namespace glm;
using namespace std;

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
		Reg<PointLight>();

		Reg<Material>();
		Reg<BSDF_Diffuse>();
		Reg<BSDF_Glass>();
		Reg<BSDF_Mirror>();
		Reg<BSDF_Emission>();
		Reg<BSDF_CookTorrance>();
		Reg<BSDF_MetalWorkflow>();
		Reg<BSDF_FrostedGlass>();

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
	void Visit(PointLight::Ptr light);

	void Visit(Material::Ptr material);
	void Visit(BSDF_Diffuse::Ptr bsdf);
	void Visit(BSDF_Emission::Ptr bsdf);
	void Visit(BSDF_Glass::Ptr bsdf);
	void Visit(BSDF_Mirror::Ptr bsdf);
	void Visit(BSDF_CookTorrance::Ptr bsdf);
	void Visit(BSDF_MetalWorkflow::Ptr bsdf);
	void Visit(BSDF_FrostedGlass::Ptr bsdf);


	void Visit(Transform::Ptr transform);

private:
	QWidget * GenItem(Component::Ptr component, const QString & str) {
		auto item = new QWidget;
		attr->component2item[component] = item;
		attr->item2component[item] = component;
		attr->componentType2item[typeid(*component)] = item;
		attr->tbox->insertItem(0, item, str);
		return item;
	}

	Grid::Ptr GetGrid(QWidget * item) {
		auto target = attr->item2grid.find(item);
		if (target != attr->item2grid.end())
			return target->second;

		auto grid = ToPtr(new Grid(item));
		attr->item2grid[item] = grid;

		return grid;
	}

private:
	Attribute * attr;
};

// -------------- Transform --------------

void Attribute::ComponentVisitor::Visit(Transform::Ptr transform) {
	auto item = GenItem(transform, "Transform");
	auto grid = GetGrid(item);

	// position
	grid->AddText("- Position");
	auto pos = transform->GetPosition();
	grid->AddEditVal("x", pos.x, 0.1, [transform](double x) {
		auto pos = transform->GetPosition();
		pos.x = x;
		transform->SetPosition(pos);
	});

	grid->AddEditVal("y", pos.y, 0.1, [transform](double y) {
		auto pos = transform->GetPosition();
		pos.y = y;
		transform->SetPosition(pos);
	});

	grid->AddEditVal("z", pos.z, 0.1, [transform](double z) {
		auto pos = transform->GetPosition();
		pos.z = z;
		transform->SetPosition(pos);
	});

	// rotation
	grid->AddText("- Rotation");
	auto rotation = degrees(transform->GetEulerRoatation());
	grid->AddEditVal("x", rotation.x, 1.0, [transform](double x) {
		auto rotation = transform->GetEulerRoatation();
		rotation.x = radians(x);
		transform->SetRotation(rotation);
	});

	grid->AddEditVal("y", rotation.y, 1.0, [transform](double y) {
		auto rotation = transform->GetEulerRoatation();
		rotation.y = radians(y);
		transform->SetRotation(rotation);
	});

	grid->AddEditVal("z", rotation.y, 1.0, [transform](double z) {
		auto rotation = transform->GetEulerRoatation();
		rotation.z = radians(z);
		transform->SetRotation(rotation);
	});

	// scale
	grid->AddText("- Scale");
	auto scale = transform->GetScale();
	grid->AddEditVal("x", scale.x, 0.1, [transform](double x) {
		auto scale = transform->GetScale();
		scale.x = x;
		transform->SetScale(scale);
	});

	grid->AddEditVal("y", scale.y, 0.1, [transform](double y) {
		auto scale = transform->GetScale();
		scale.y = y;
		transform->SetScale(scale);
	});

	grid->AddEditVal("z", scale.z, 0.1, [transform](double z) {
		auto scale = transform->GetScale();
		scale.z = z;
		transform->SetScale(scale);
	});

}

// -------------- Camera --------------

void Attribute::ComponentVisitor::Visit(Camera::Ptr camera) {
	auto item = GenItem(camera, "Camera");
	auto grid = GetGrid(item);
	
	grid->AddEditVal("- Field of View", camera->GetFOV(), 1, 179, [camera](double fov) {
		camera->SetFOV(fov);
	});
}

// -------------- Geometry --------------

void Attribute::ComponentVisitor::Visit(Geometry::Ptr geo) {
	auto item = GenItem(geo, "Geometry");
	auto grid = GetGrid(item);

	auto getTypeStr = ToPtr(new EleVisitor);
	getTypeStr->Reg<Sphere>([=](Sphere::Ptr) {
		getTypeStr->RegArg("typeStr", "Sphere");
	});
	getTypeStr->Reg<Plane>([=](Plane::Ptr) {
		getTypeStr->RegArg("typeStr", "Plane");
	});
	getTypeStr->Reg<TriMesh>([=](TriMesh::Ptr) {
		getTypeStr->RegArg("typeStr", "TriMesh");
	});

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);
	QComboBox * combobox = new QComboBox;

	(*pSlotMap)["None"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "None", pSlotMap);
		geo->SetPrimitive(nullptr);
	};
	(*pSlotMap)["Sphere"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "Sphere", pSlotMap);

		auto sphere = ToPtr(new Sphere);
		geo->SetPrimitive(sphere);
		Visit(sphere);
	};
	(*pSlotMap)["Plane"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "Plane", pSlotMap);

		auto plane = ToPtr(new Plane);
		geo->SetPrimitive(plane);
		Visit(plane);
	};
	(*pSlotMap)["TriMesh"] = [=]() {
		if (geo->GetPrimitive()) {
			geo->GetPrimitive()->Accept(getTypeStr);
			auto typeStr = getTypeStr->GetArg<string>("typeStr");
			combobox->setCurrentText(QString::fromStdString(typeStr));
		}
		else
			combobox->setCurrentText("None");
	};

	if (geo->GetPrimitive()) {
		geo->GetPrimitive()->Accept(getTypeStr);
		grid->AddComboBox(combobox, "Type", getTypeStr->GetArg<string>("typeStr"), pSlotMap);

		geo->GetPrimitive()->Accept(This());
	}
	else
		grid->AddComboBox(combobox, "Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(Sphere::Ptr sphere) {
	auto grid = GetGrid(attr->componentType2item[typeid(Geometry)]);
	auto & center = sphere->center;
	auto & r = sphere->r;

	grid->AddText("- center");
	grid->AddEditVal("x", center.x, 0.1);
	grid->AddEditVal("y", center.y, 0.1);
	grid->AddEditVal("z", center.z, 0.1);

	grid->AddEditVal("- radius", r, 0.1);
}

void Attribute::ComponentVisitor::Visit(Plane::Ptr plane) {
	auto grid = GetGrid(attr->componentType2item[typeid(Geometry)]);
	grid->AddText("empty");
}

void Attribute::ComponentVisitor::Visit(TriMesh::Ptr mesh) {
	auto grid = GetGrid(attr->componentType2item[typeid(Geometry)]);
	grid->AddText("- Triangle", mesh->GetIndice().size() / 3);
	grid->AddText("- Vertex", mesh->GetPositions().size());
}

// -------------- Material --------------

void Attribute::ComponentVisitor::Visit(Material::Ptr material) {
	auto item = GenItem(material, "Material");
	auto grid = GetGrid(item);

	auto getTypeStr = ToPtr(new EleVisitor);
	getTypeStr->Reg<BSDF_Diffuse>([=](BSDF_Diffuse::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_Diffuse");
	});
	getTypeStr->Reg<BSDF_Emission>([=](BSDF_Emission::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_Emission");
	});
	getTypeStr->Reg<BSDF_Glass>([=](BSDF_Glass::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_Glass");
	});
	getTypeStr->Reg<BSDF_Mirror>([=](BSDF_Mirror::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_Mirror");
	});
	getTypeStr->Reg<BSDF_CookTorrance>([=](BSDF_CookTorrance::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_CookTorrance");
	});
	getTypeStr->Reg<BSDF_MetalWorkflow>([=](BSDF_MetalWorkflow::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_MetalWorkflow");
	});
	getTypeStr->Reg<BSDF_FrostedGlass>([=](BSDF_FrostedGlass::Ptr) {
		getTypeStr->RegArg("typeStr", "BSDF_FrostedGlass");
	});

	const int bsdfNum = 8;
	tuple<string, function<BSDF::Ptr()>> bsdfArr[bsdfNum] = {
		{"None", []()->BSDF::Ptr { return nullptr; } },
		{"BSDF_Diffuse", []()->BSDF::Ptr { return ToPtr(new BSDF_Diffuse); } },
		{"BSDF_Emission", []()->BSDF::Ptr { return ToPtr(new BSDF_Emission); } },
		{"BSDF_Glass", []()->BSDF::Ptr { return ToPtr(new BSDF_Glass); } },
		{"BSDF_Mirror", []()->BSDF::Ptr { return ToPtr(new BSDF_Mirror); } },
		{"BSDF_CookTorrance", []()->BSDF::Ptr { return ToPtr(new BSDF_CookTorrance(1.5f,0.2f)); } },
		{"BSDF_MetalWorkflow", []()->BSDF::Ptr { return ToPtr(new BSDF_MetalWorkflow); } },
		{"BSDF_FrostedGlass", []()->BSDF::Ptr { return ToPtr(new BSDF_FrostedGlass); } },
	};

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);

	for (int i = 0; i < bsdfNum; i++) {
		(*pSlotMap)[get<0>(bsdfArr[i])] = [=]() {
			grid->Clear();
			grid->AddComboBox("Type", get<0>(bsdfArr[i]), pSlotMap);

			auto bsdf = get<1>(bsdfArr[i])();
			material->SetMat(bsdf);
			if(bsdf != nullptr)
				bsdf->Accept(This());
		};
	}

	if (material->GetMat()) {
		material->GetMat()->Accept(getTypeStr);
		grid->AddComboBox("Type", getTypeStr->GetArg<string>("typeStr"), pSlotMap);

		material->GetMat()->Accept(This());
	}
	else
		grid->AddComboBox("Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(BSDF_Diffuse::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);
	grid->AddEditColor("- Albedo", bsdf->albedoColor);
	grid->AddEditImage("- Albedo Texture", bsdf->albedoTexture);
}

void Attribute::ComponentVisitor::Visit(BSDF_Emission::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);
	grid->AddEditColor("- Color", bsdf->color);
	grid->AddEditVal("- Intensity", bsdf->intensity, 0, 10, 1000);
}

void Attribute::ComponentVisitor::Visit(BSDF_Glass::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);
	grid->AddEditColor("- Transmittance", bsdf->transmittance);
	grid->AddEditColor("- Reflectance", bsdf->reflectance);
	grid->AddEditVal("- ior", bsdf->ior, 0.01);
}

void Attribute::ComponentVisitor::Visit(BSDF_Mirror::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);
	grid->AddEditColor("- Reflectance", bsdf->reflectance);
}

void Attribute::ComponentVisitor::Visit(BSDF_CookTorrance::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);
	grid->AddEditColor("- Reflectance", bsdf->refletance);
	grid->AddEditColor("- Albedo", bsdf->albedo);
	grid->AddEditVal("- Index of Refract", bsdf->ior, 0.01);
	grid->AddEditVal("- Roughness", bsdf->m, 0, 1, 100);
}

void Attribute::ComponentVisitor::Visit(BSDF_MetalWorkflow::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);

	grid->AddEditColor("- Albedo Color", bsdf->albedoColor);
	grid->AddEditImage("- Albedo Texture", bsdf->GetAlbedoTexture(),
		[=](CppUtil::Basic::Ptr<Image> img) {bsdf->SetAlbedoTexture(img); });

	grid->AddEditVal("- Metallic Factor", bsdf->metallicFactor, 0, 1, 100);
	grid->AddEditImage("- Metallic Texture", bsdf->GetMetallicTexture(),
		[=](CppUtil::Basic::Ptr<Image> img) {bsdf->SetMetallicTexture(img); });

	grid->AddEditVal("- Roughness Factor", bsdf->roughnessFactor, 0, 1, 100);
	grid->AddEditImage("- Roughness Texture", bsdf->GetRoughnessTexture(),
		[=](CppUtil::Basic::Ptr<Image> img) {bsdf->SetRoughnessTexture(img); });

	grid->AddEditImage("- AO Texture", bsdf->GetAOTexture(),
		[=](CppUtil::Basic::Ptr<Image> img) {bsdf->SetAOTexture(img); });

	grid->AddEditImage("- Normal Texture", bsdf->GetNormalTexture(),
		[=](CppUtil::Basic::Ptr<Image> img) {bsdf->SetNormalTexture(img); });
}

void Attribute::ComponentVisitor::Visit(BSDF_FrostedGlass::Ptr bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(Material)]);

	grid->AddEditColor("- Albedo", bsdf->albedo);
	grid->AddEditVal("- IOR", bsdf->ior, 0., 20., 200);
	grid->AddEditVal("- Alpha", bsdf->alpha, 0., 1., 100);
}

// -------------- Light --------------

void Attribute::ComponentVisitor::Visit(Light::Ptr light) {
	auto item = GenItem(light, "Light");
	auto grid = GetGrid(item);

	auto getTypeStr = ToPtr(new EleVisitor);
	getTypeStr->Reg<AreaLight>([=](AreaLight::Ptr) {
		getTypeStr->RegArg("typeStr", "AreaLight");
	});
	getTypeStr->Reg<PointLight>([=](PointLight::Ptr) {
		getTypeStr->RegArg("typeStr", "PointLight");
	});

	const int lightNum = 3;
	tuple<string, function<LightBase::Ptr()>> lightArr[lightNum] = {
		{"None", []()->LightBase::Ptr { return nullptr; } },
		{"AreaLight", []()->LightBase::Ptr { return ToPtr(new AreaLight); } },
		{"PointLight", []()->LightBase::Ptr { return ToPtr(new PointLight); } },
	};

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);

	for (int i = 0; i < lightNum; i++) {
		(*pSlotMap)[get<0>(lightArr[i])] = [=]() {
			grid->Clear();
			grid->AddComboBox("Type", get<0>(lightArr[i]), pSlotMap);

			auto lightbase = get<1>(lightArr[i])();
			light->SetLight(lightbase);
			if (lightbase != nullptr)
				lightbase->Accept(This());
		};
	}

	if (light->GetLight()) {
		light->GetLight()->Accept(getTypeStr);
		grid->AddComboBox("Type", getTypeStr->GetArg<string>("typeStr"), pSlotMap);

		light->GetLight()->Accept(This());
	}
	else
		grid->AddComboBox("Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(AreaLight::Ptr light) {
	auto grid = GetGrid(attr->componentType2item[typeid(Light)]);
	grid->AddEditColor("- Color", light->color);
	grid->AddEditVal("- Intensity", light->intensity, 0.1);
	grid->AddEditVal("- Width", light->width, 0.1);
	grid->AddEditVal("- Height", light->height, 0.1);
}

void Attribute::ComponentVisitor::Visit(PointLight::Ptr light) {
	auto grid = GetGrid(attr->componentType2item[typeid(Light)]);
	grid->AddEditColor("- Color", light->color);
	grid->AddEditVal("- Intensity", light->intensity, 0, 20, 2000);
	grid->AddEditVal("- Linear", light->linear, 0, 1.0, 100);
	grid->AddEditVal("- Quadratic", light->quadratic, 0, 2.0, 200);
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
	item2grid.clear();

	if (sobj == nullptr)
		return;

	for (auto component : sobj->GetAllComponents())
		component->Accept(visitor);

	AddController();
}

void Attribute::AddController() {
	auto item = new QWidget;
	tbox->insertItem(tbox->count(), item, "Controller");

	auto grid = ToPtr(new Grid(item));
	item2grid[item] = grid;

	grid->AddTitle("[ Component ]");

	const int componentNum = 5;
	vector<string> componentNames{ "Camera" , "Geometry", "Light", "Material", "Transform" };

	vector<function<Component::Ptr()>> componentGenFuncs{
		[=]()->Component::Ptr { return ToPtr(new Camera(nullptr)); },
		[=]()->Component::Ptr { return ToPtr(new Geometry(nullptr, nullptr)); },
		[=]()->Component::Ptr { return ToPtr(new Light(nullptr, nullptr)); },
		[=]()->Component::Ptr { return ToPtr(new Material(nullptr, nullptr)); },
		[=]()->Component::Ptr { return ToPtr(new Transform(nullptr)); },
	};

	vector<function<Component::Ptr()>> componentDelFuncs{
		[=]()->Component::Ptr {
			auto component = sobj->GetComponent<Camera>();
			sobj->DetachComponent<Camera>();
			return component;
		},
		[=]()->Component::Ptr {
			auto component = sobj->GetComponent<Geometry>();
			sobj->DetachComponent<Geometry>();
			return component;
		},
		[=]()->Component::Ptr {
			auto component = sobj->GetComponent<Light>();
			sobj->DetachComponent<Light>();
			return component;
		},
		[=]()->Component::Ptr {
			auto component = sobj->GetComponent<Material>();
			sobj->DetachComponent<Material>();
			return component;
		},
		[=]()->Component::Ptr {
			auto component = sobj->GetComponent<Transform>();
			sobj->DetachComponent<Transform>();
			return component;
		},
	};

	map<std::string, function<Component::Ptr()> > componentGenMap;
	map<std::string, function<Component::Ptr()> > componentDelMap;
	for (int i = 0; i < componentNum; i++) {
		componentGenMap[componentNames[i]] = componentGenFuncs[i];
		componentDelMap[componentNames[i]] = componentDelFuncs[i];
	}

	auto addBtnSlot = [=](const string & item) {
		auto target = componentGenMap.find(item);
		if (target == componentGenMap.cend())
			return;

		auto component = target->second();
		if (sobj->HaveComponentSameTypeWith(component))
			return;

		component->AttachSObj(sobj);
		component->Accept(visitor);
	};

	auto delBtnSlot = [=](const string & item) {
		auto target = componentDelMap.find(item);
		if (target == componentDelMap.cend())
			return;

		auto component = target->second();
		if (component == nullptr)
			return;

		auto target2 = component2item.find(component);
		if (target2 == component2item.end())
			return;

		auto tboxItem = target2->second;

		component2item.erase(component);
		item2component.erase(tboxItem);
		item2grid.erase(tboxItem);

		tbox->removeItem(tbox->indexOf(tboxItem));
		delete tboxItem;
	};

	grid->AddComboBox("- Add Component", componentNames, "Add", addBtnSlot);
	grid->AddComboBox("- Delete Component", componentNames, "Delete", delBtnSlot);
}
