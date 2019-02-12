#include <UI/Attribute.h>

#include <UI/Grid.h>

#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/AllComponents.h>

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
		Reg<Light>();
		Reg<Material>();
		Reg<Transform>();
	}

private:
	void Visit(Camera::Ptr camera) {
		auto item = new QWidget;
		AddItem(camera, item, "Camera");
	}

	void Visit(Geometry::Ptr geo) {
		auto item = new QWidget;
		AddItem(geo, item, "Geometry");
	}

	void Visit(Light::Ptr light) {
		auto item = new QWidget;
		AddItem(light, item, "Light");
	}

	void Visit(Material::Ptr material) {
		auto item = new QWidget;
		AddItem(material, item, "Material");
	}

	void Visit(Transform::Ptr transform);

private:
	void AddItem(Component::Ptr component, QWidget * item, QString str) {
		attr->component2item[component] = item;
		attr->item2component[item] = component;
		attr->tbox->addItem(item, str);
	}

private:
	Attribute * attr;
};

void Attribute::ComponentVisitor::Visit(Transform::Ptr transform) {
	auto item = new QWidget;
	auto vboxLayout = new QVBoxLayout(item);
	auto gridLayout = new QGridLayout();
	auto verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	vboxLayout->addLayout(gridLayout);
	vboxLayout->addItem(verticalSpacer);
	AddItem(transform, item, "Transform");

	auto grid = Grid(item, gridLayout);

	// position
	grid.AddTitle("Position");
	auto pos = transform->GetPosition();
	auto posX = grid.AddLine("x", pos.x, 0.1, [transform](double x) {
		auto pos = transform->GetPosition();
		pos.x = x;
		transform->SetPosition(pos);
	});

	auto posY = grid.AddLine("y", pos.y, 0.1, [transform](double y) {
		auto pos = transform->GetPosition();
		pos.y = y;
		transform->SetPosition(pos);
	});

	auto posZ = grid.AddLine("z", pos.z, 0.1, [transform](double z) {
		auto pos = transform->GetPosition();
		pos.z = z;
		transform->SetPosition(pos);
	});

	// rotation
	grid.AddTitle("Rotation");
	auto rotation = degrees(transform->GetEulerRoatation());
	auto rX = grid.AddLine("x", rotation.x, 1.0, [transform](double x) {
		auto rotation = transform->GetEulerRoatation();
		rotation.x = radians(x);
		transform->SetRotation(rotation);
	});

	auto rY = grid.AddLine("y", rotation.y, 1.0, [transform](double y) {
		auto rotation = transform->GetEulerRoatation();
		rotation.y = radians(y);
		transform->SetRotation(rotation);
	});

	auto rZ = grid.AddLine("z", rotation.y, 1.0, [transform](double z) {
		auto rotation = transform->GetEulerRoatation();
		rotation.z = radians(z);
		transform->SetRotation(rotation);
	});

	// scale
	grid.AddTitle("Scale");
	auto scale = transform->GetScale();
	auto scaleX = grid.AddLine("x", scale.x, 0.1, [transform](double x) {
		auto scale = transform->GetScale();
		scale.x = x;
		transform->SetScale(scale);
	});

	auto scaleY = grid.AddLine("y", scale.y, 0.1, [transform](double y) {
		auto scale = transform->GetScale();
		scale.y = y;
		transform->SetScale(scale);
	});

	auto scaleZ = grid.AddLine("z", scale.z, 0.1, [transform](double z) {
		auto scale = transform->GetScale();
		scale.z = z;
		transform->SetScale(scale);
	});

}

Attribute::Attribute(QToolBox * tbox)
	: tbox(tbox), sobj(nullptr), visitor(ToPtr(new ComponentVisitor(this))){
	SetSObj(nullptr);
}

void Attribute::SetSObj(SObj::Ptr sobj) {
	this->sobj = sobj;

	// clear
	int num = tbox->count();
	while (num-- > 0) {
		// remove item ²»»áÉ¾³ý item
		auto item = tbox->widget(0);
		tbox->removeItem(0);
		delete item;
	}

	if (sobj == nullptr)
		return;

	for (auto component : sobj->GetAllComponents())
		component->Accept(visitor);
}
