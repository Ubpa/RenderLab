#include <CppUtil/Engine/CmptTransform.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

CmptTransform::CmptTransform(Basic::Ptr<SObj> sobj)
	: Component(sobj), transform(1), position(0), scale(1), rotation(), dirtyTransform(false)  { }

const Transform & CmptTransform::GetTransform() {
	UpdateMat();
	return transform;
}

void CmptTransform::SetPosition(const Pointf & position) {
	dirtyTransform = true;
	this->position = position;
}

void CmptTransform::SetRotation(const Quatf & rotation) {
	dirtyTransform = true;
	this->rotation = rotation;
}

void CmptTransform::SetScale(const Val3f & scale) {
	dirtyTransform = true;
	this->scale = scale;
}

void CmptTransform::UpdateMat() {
	if (dirtyTransform) {
		const auto scaleTsfm = Transform::Scale(scale);
		const auto rotateTsfm = Transform::Rotate(rotation);
		const auto translateTsfm = Transform::Translate(position);
		transform = translateTsfm * rotateTsfm * scaleTsfm;

		dirtyTransform = false;
	}
}

void CmptTransform::SetTransform(const Transform & transform) {
	this->transform = transform;
	position = transform.Position();
	rotation = transform.RotationQuat();
	scale = transform.Scale();
	dirtyTransform = false;
}
