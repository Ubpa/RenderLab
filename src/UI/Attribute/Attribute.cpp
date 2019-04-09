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

#include <CppUtil/Basic/Visitor.h>
#include <CppUtil/Basic/Math.h>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QLabel>
#include <qcombobox.h>

#include <tuple>

using namespace Ui;
using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;
using namespace std;

class Attribute::ComponentVisitor : public Visitor {
public:
	ComponentVisitor(Attribute * attr)
		: attr(attr) {
		RegMemberFunc<CmptCamera>(&Attribute::ComponentVisitor::Visit);

		RegMemberFunc<CmptGeometry>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<Sphere>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<Plane>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<TriMesh>(&Attribute::ComponentVisitor::Visit);

		RegMemberFunc<CmptLight>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<AreaLight>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<PointLight>(&Attribute::ComponentVisitor::Visit);

		RegMemberFunc<CmptMaterial>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_Diffuse>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_Glass>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_Mirror>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_Emission>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_CookTorrance>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_MetalWorkflow>(&Attribute::ComponentVisitor::Visit);
		RegMemberFunc<BSDF_FrostedGlass>(&Attribute::ComponentVisitor::Visit);

		RegMemberFunc<CmptTransform>(&Attribute::ComponentVisitor::Visit);
	}

public:
	static const Basic::Ptr<ComponentVisitor> New(Attribute * attr) {
		return Basic::New<ComponentVisitor>(attr);
	}

protected:
	virtual ~ComponentVisitor() = default;

public:
	void Visit(Ptr<CmptCamera> cmpt);

	void Visit(Ptr<CmptGeometry> cmpt);
	void Visit(Ptr<Sphere> sphere);
	void Visit(Ptr<Plane> plane);
	void Visit(Ptr<TriMesh> mesh);

	void Visit(Ptr<CmptLight> cmpt);
	void Visit(Ptr<AreaLight> light);
	void Visit(Ptr<PointLight> light);

	void Visit(Ptr<CmptMaterial> cmpt);
	void Visit(Ptr<BSDF_Diffuse> bsdf);
	void Visit(Ptr<BSDF_Emission> bsdf);
	void Visit(Ptr<BSDF_Glass> bsdf);
	void Visit(Ptr<BSDF_Mirror> bsdf);
	void Visit(Ptr<BSDF_CookTorrance> bsdf);
	void Visit(Ptr<BSDF_MetalWorkflow> bsdf);
	void Visit(Ptr<BSDF_FrostedGlass> bsdf);


	void Visit(Ptr<CmptTransform> cmpt);

private:
	QWidget * GenItem(Ptr<Component> component, const QString & str) {
		auto item = new QWidget;
		attr->componentType2item[typeid(*component)] = item;
		attr->tbox->insertItem(0, item, str);
		return item;
	}

	Ptr<Grid> GetGrid(QWidget * item) {
		auto target = attr->item2grid.find(item);
		if (target != attr->item2grid.end())
			return target->second;

		auto grid = Grid::New(item);
		attr->item2grid[item] = grid;

		return grid;
	}

private:
	Attribute * attr;
};

// -------------- Transform --------------

void Attribute::ComponentVisitor::Visit(Ptr<CmptTransform> transform) {
	auto item = GenItem(transform, "Transform");
	auto grid = GetGrid(item);

	// position
	grid->AddText("- Position");
	grid->AddEditVal({ "x","y","z" }, transform->GetPosition(), Val3(0.1f), [=](const Val3 & val) {
		transform->SetPosition(val);
	});

	grid->AddText("- Rotation");
	grid->AddEditVal({"x","y","z"}, transform->GetRotationEuler(), Vec3(-90, -180, -180), Vec3(90, 180, 180), Vec3i(180, 360, 360), [=](const Val3 & val) {
		transform->SetRotation(EulerYXZf(val));
	});

	grid->AddText("- Scale");
	grid->AddEditVal({ "x","y","z" }, transform->GetScale(), Val3(0.1f), [=](const Val3 & val) {
		transform->SetScale(val);
	});
}

// -------------- Camera --------------

void Attribute::ComponentVisitor::Visit(Ptr<CmptCamera> camera) {
	auto item = GenItem(camera, "Camera");
	auto grid = GetGrid(item);
	
	grid->AddEditVal("- Field of View", camera->GetFOV(), 1, 179, [camera](double fov) {
		camera->SetFOV(fov);
	});
}

// -------------- Geometry --------------

void Attribute::ComponentVisitor::Visit(Ptr<CmptGeometry> geo) {
	auto item = GenItem(geo, "Geometry");
	auto grid = GetGrid(item);

	string typeStr;
	auto getTypeStr = Visitor::New();
	getTypeStr->Reg([&typeStr](Ptr<Sphere>) {
		typeStr = "Sphere";
	});
	getTypeStr->Reg([&typeStr](Ptr<Plane>) {
		typeStr = "Plane";
	});
	getTypeStr->Reg([&typeStr](Ptr<TriMesh>) {
		typeStr = "TriMesh";
	});

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);
	Grid::wpSlotMap wpSlotMap = pSlotMap;

	QComboBox * combobox = new QComboBox;

	(*pSlotMap)["None"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "None", wpSlotMap.lock());
		geo->primitive = nullptr;
	};
	(*pSlotMap)["Sphere"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "Sphere", wpSlotMap.lock());

		auto sphere = Sphere::New();
		geo->primitive = sphere;
		Visit(sphere);
	};
	(*pSlotMap)["Plane"] = [=]() {
		grid->Clear();
		grid->AddComboBox("Type", "Plane", wpSlotMap.lock());

		auto plane = Plane::New();
		geo->primitive = plane;
		Visit(plane);
	};
	(*pSlotMap)["TriMesh"] = [=]() {
		if (geo->primitive) {
			geo->primitive->Accept(getTypeStr);
			combobox->setCurrentText(QString::fromStdString(typeStr));
		}
		else
			combobox->setCurrentText("None");
	};

	if (geo->primitive) {
		geo->primitive->Accept(getTypeStr);
		grid->AddComboBox(combobox, "Type", typeStr, pSlotMap);

		geo->primitive->Accept(This());
	}
	else
		grid->AddComboBox(combobox, "Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(Ptr<Sphere> sphere) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptGeometry)]);
	grid->AddText("empty");
}

void Attribute::ComponentVisitor::Visit(Ptr<Plane> plane) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptGeometry)]);
	grid->AddText("empty");
}

void Attribute::ComponentVisitor::Visit(Ptr<TriMesh> mesh) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptGeometry)]);
	grid->AddText("- Triangle", mesh->GetIndice().size() / 3);
	grid->AddText("- Vertex", mesh->GetPositions().size());
}

// -------------- Material --------------

void Attribute::ComponentVisitor::Visit(Ptr<CmptMaterial> cmpt) {
	auto item = GenItem(cmpt, "Material");
	auto grid = GetGrid(item);

	string typeStr;
	auto getTypeStr = Visitor::New();
	getTypeStr->Reg([&typeStr](Ptr<BSDF_Diffuse>) {
		typeStr = "BSDF_Diffuse";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_Emission>) {
		typeStr = "BSDF_Emission";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_Glass>) {
		typeStr = "BSDF_Glass";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_Mirror>) {
		typeStr = "BSDF_Mirror";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_CookTorrance>) {
		typeStr = "BSDF_CookTorrance";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_MetalWorkflow>) {
		typeStr = "BSDF_MetalWorkflow";
	});
	getTypeStr->Reg([&typeStr](Ptr<BSDF_FrostedGlass>) {
		typeStr = "BSDF_FrostedGlass";
	});

	const int bsdfNum = 8;
	tuple<string, function<Ptr<BSDF>()>> bsdfArr[bsdfNum] = {
		{"None", []()->Ptr<BSDF> { return nullptr; } },
		{"BSDF_Diffuse", []()->Ptr<BSDF> { return BSDF_Diffuse::New(); } },
		{"BSDF_Emission", []()->Ptr<BSDF> { return BSDF_Emission::New(); } },
		{"BSDF_Glass", []()->Ptr<BSDF> { return BSDF_Glass::New(); } },
		{"BSDF_Mirror", []()->Ptr<BSDF> { return BSDF_Mirror::New(); } },
		{"BSDF_CookTorrance", []()->Ptr<BSDF> { return BSDF_CookTorrance::New(); } },
		{"BSDF_MetalWorkflow", []()->Ptr<BSDF> { return BSDF_MetalWorkflow::New(); } },
		{"BSDF_FrostedGlass", []()->Ptr<BSDF> { return BSDF_FrostedGlass::New(); } },
	};

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);
	Grid::wpSlotMap wpSlotMap = pSlotMap;

	for (int i = 0; i < bsdfNum; i++) {
		(*pSlotMap)[get<0>(bsdfArr[i])] = [=]() {
			grid->Clear();
			grid->AddComboBox("Type", get<0>(bsdfArr[i]), wpSlotMap.lock());

			auto bsdf = get<1>(bsdfArr[i])();
			cmpt->material = bsdf;
			if(bsdf != nullptr)
				bsdf->Accept(This());
		};
	}

	if (cmpt->material) {
		cmpt->material->Accept(getTypeStr);
		grid->AddComboBox("Type", typeStr, pSlotMap);

		cmpt->material->Accept(This());
	}
	else
		grid->AddComboBox("Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_Diffuse> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);
	grid->AddEditColor("- Albedo", bsdf->colorFactor);
	grid->AddEditImage("- Albedo Texture", bsdf->albedoTexture);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_Emission> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);
	grid->AddEditColor("- Color", bsdf->color);
	grid->AddEditVal("- Intensity", bsdf->intensity, 0, 10, 1000);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_Glass> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);
	grid->AddEditColor("- Transmittance", bsdf->transmittance);
	grid->AddEditColor("- Reflectance", bsdf->reflectance);
	grid->AddEditVal("- ior", bsdf->ior, 0.01);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_Mirror> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);
	grid->AddEditColor("- Reflectance", bsdf->reflectance);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_CookTorrance> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);
	grid->AddEditColor("- Reflectance", bsdf->refletance);
	grid->AddEditColor("- Albedo", bsdf->albedo);
	grid->AddEditVal("- Index of Refract", bsdf->ior, 0.01);
	grid->AddEditVal("- Roughness", bsdf->m, 0, 1, 100);
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_MetalWorkflow> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);

	grid->AddEditColor("- Albedo Color", bsdf->colorFactor);
	grid->AddEditImage("- Albedo Texture", bsdf->GetAlbedoTexture(),
		[=](Ptr<Image> img) {bsdf->SetAlbedoTexture(img); });

	grid->AddEditVal("- Metallic Factor", bsdf->metallicFactor, 0, 1, 100);
	grid->AddEditImage("- Metallic Texture", bsdf->GetMetallicTexture(),
		[=](Ptr<Image> img) {bsdf->SetMetallicTexture(img); });

	grid->AddEditVal("- Roughness Factor", bsdf->roughnessFactor, 0, 1, 100);
	grid->AddEditImage("- Roughness Texture", bsdf->GetRoughnessTexture(),
		[=](Ptr<Image> img) {bsdf->SetRoughnessTexture(img); });

	grid->AddEditImage("- AO Texture", bsdf->GetAOTexture(),
		[=](Ptr<Image> img) {bsdf->SetAOTexture(img); });

	grid->AddEditImage("- Normal Texture", bsdf->GetNormalTexture(),
		[=](Ptr<Image> img) {bsdf->SetNormalTexture(img); });
}

void Attribute::ComponentVisitor::Visit(Ptr<BSDF_FrostedGlass> bsdf) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptMaterial)]);

	grid->AddEditVal("- IOR", bsdf->ior, 1., 20., 190);

	grid->AddEditColor("- Color Factor", bsdf->colorFactor);
	grid->AddEditImage("- Color Texture", bsdf->colorTexture);

	grid->AddEditVal("- Roughness Factor", bsdf->roughnessFactor, 0.02, 1., 98);
	grid->AddEditImage("- Roughness Texture", bsdf->roughnessTexture);

	grid->AddEditImage("- AO Texture", bsdf->aoTexture);
	grid->AddEditImage("- Normal Texture", bsdf->normalTexture);
}

// -------------- Light --------------

void Attribute::ComponentVisitor::Visit(Ptr<CmptLight> cmpt) {
	auto item = GenItem(cmpt, "Light");
	auto grid = GetGrid(item);

	auto getTypeStr = Visitor::New();
	string typeStr;
	getTypeStr->Reg([&typeStr](Ptr<AreaLight>) {
		typeStr = "AreaLight";
	});
	getTypeStr->Reg([&typeStr](Ptr<PointLight>) {
		typeStr = "PointLight";
	});

	const int lightNum = 3;
	tuple<string, function<Ptr<Light>()>> lightArr[lightNum] = {
		{"None", []()->Ptr<Light> { return nullptr; } },
		{"AreaLight", []()->Ptr<Light> { return AreaLight::New(); } },
		{"PointLight", []()->Ptr<Light> { return PointLight::New(); } },
	};

	Grid::pSlotMap pSlotMap(new Grid::SlotMap);
	Grid::wpSlotMap wpSlotMap = pSlotMap;

	for (int i = 0; i < lightNum; i++) {
		(*pSlotMap)[get<0>(lightArr[i])] = [=]() {
			grid->Clear();
			grid->AddComboBox("Type", get<0>(lightArr[i]), wpSlotMap.lock());

			auto lightbase = get<1>(lightArr[i])();
			cmpt->light = lightbase;
			if (lightbase != nullptr)
				lightbase->Accept(This());
		};
	}

	if (cmpt->light) {
		cmpt->light->Accept(getTypeStr);
		grid->AddComboBox("Type", typeStr, pSlotMap);

		cmpt->light->Accept(This());
	}
	else
		grid->AddComboBox("Type", "None", pSlotMap);
}

void Attribute::ComponentVisitor::Visit(Ptr<AreaLight> light) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptLight)]);
	grid->AddEditColor("- Color", light->color);
	grid->AddEditVal("- Intensity", light->intensity, 0.1);
	grid->AddEditVal("- Width", light->width, 0.1);
	grid->AddEditVal("- Height", light->height, 0.1);
}

void Attribute::ComponentVisitor::Visit(Ptr<PointLight> light) {
	auto grid = GetGrid(attr->componentType2item[typeid(CmptLight)]);
	grid->AddEditColor("- Color", light->color);
	grid->AddEditVal("- Intensity", light->intensity, 0, 20, 2000);
	grid->AddEditVal("- Linear", light->linear, 0, 1.0, 100);
	grid->AddEditVal("- Quadratic", light->quadratic, 0, 2.0, 200);
}

// -------------- Attribute --------------

Attribute::Attribute()
	: tbox(nullptr), visitor(ComponentVisitor::New(this)){
}

void Attribute::Init(QToolBox * tbox) {
	this->tbox = tbox;
	SetSObj(nullptr);
}

void Attribute::SetSObj(Ptr<SObj> sobj) {
	if (!tbox)
		return;

	// clear
	componentType2item.clear();

	for (auto pair : item2grid)
		pair.second->Clear();
	item2grid.clear();

	// 一个 component 对应一个 widget
	int num = tbox->count();
	while (num-- > 0) {
		// remove item 不会删除 item
		auto item = tbox->widget(0);
		tbox->removeItem(0);
		item->deleteLater();
	}

	if (sobj == nullptr)
		return;

	for (auto component : sobj->GetAllComponents())
		component->Accept(visitor);

	AddController(sobj);
}

void Attribute::AddController(Ptr<SObj> sobj) {
	auto item = new QWidget;
	tbox->insertItem(tbox->count(), item, "Controller");

	auto grid = Grid::New(item);
	item2grid[item] = grid;

	grid->AddTitle("[ Component ]");

	const int componentNum = 5;
	vector<string> componentNames{ "Camera" , "Geometry", "Light", "Material", "Transform" };

	vector<function<Ptr<Component>()>> componentGenFuncs{
		[=]()->Ptr<Component> { return CmptCamera::New(nullptr); },
		[=]()->Ptr<Component> { return CmptGeometry::New(nullptr, nullptr); },
		[=]()->Ptr<Component> { return CmptLight::New(nullptr, nullptr); },
		[=]()->Ptr<Component> { return CmptMaterial::New(nullptr, nullptr); },
		[=]()->Ptr<Component> { return CmptTransform::New(nullptr); },
	};

	vector<function<Ptr<Component>()>> componentDelFuncs{
		[=]()->Ptr<Component> {
			auto component = sobj->GetComponent<CmptCamera>();
			sobj->DetachComponent<CmptCamera>();
			return component;
		},
		[=]()->Ptr<Component> {
			auto component = sobj->GetComponent<CmptGeometry>();
			sobj->DetachComponent<CmptGeometry>();
			return component;
		},
		[=]()->Ptr<Component> {
			auto component = sobj->GetComponent<CmptLight>();
			sobj->DetachComponent<CmptLight>();
			return component;
		},
		[=]()->Ptr<Component> {
			auto component = sobj->GetComponent<CmptMaterial>();
			sobj->DetachComponent<CmptMaterial>();
			return component;
		},
		[=]()->Ptr<Component> {
			auto component = sobj->GetComponent<CmptTransform>();
			sobj->DetachComponent<CmptTransform>();
			return component;
		},
	};

	map<std::string, function<Ptr<Component>()> > componentGenMap;
	map<std::string, function<Ptr<Component>()> > componentDelMap;
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

		sobj->AttachComponent(component);
		component->Accept(visitor);
	};

	auto delBtnSlot = [=](const string & item) {
		auto target = componentDelMap.find(item);
		if (target == componentDelMap.cend())
			return;

		auto component = target->second();
		if (component == nullptr)
			return;

		auto target2 = componentType2item.find(typeid(*component));
		if (target2 == componentType2item.cend())
			return;

		auto tboxItem = target2->second;

		componentType2item.erase(target2);

		item2grid.erase(tboxItem);

		tbox->removeItem(tbox->indexOf(tboxItem));
		delete tboxItem;
	};

	grid->AddComboBox("- Add Component", componentNames, "Add", addBtnSlot);
	grid->AddComboBox("- Delete Component", componentNames, "Delete", delBtnSlot);
}
