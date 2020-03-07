#include <Engine/Scene/CmptTransform.h>

using namespace Ubpa;

void CmptTransform::Init(const pointf3 & pos, const scalef3 & scale, const quatf & rot) {
	position = pos;
	this->scale = scale;
	rotation = rot;
	dirtyTransform = true;
}

const transformf & CmptTransform::GetTransform() const{
	UpdateMat();
	return transform;
}

void CmptTransform::SetPosition(const pointf3 & position) {
	dirtyTransform = true;
	this->position = position;
}

void CmptTransform::SetRotation(const quatf & rotation) {
	dirtyTransform = true;
	this->rotation = rotation;
}

void CmptTransform::SetScale(const scalef3& scale) {
	dirtyTransform = true;
	this->scale = scale;
}

void CmptTransform::UpdateMat() const{
	if (dirtyTransform) {
		const auto scaleTsfm = transformf(scale);
		const auto rotateTsfm = transformf(rotation);
		const auto translateTsfm = transformf(position.cast_to<vecf3>());
		transform = translateTsfm * rotateTsfm * scaleTsfm;

		dirtyTransform = false;
	}
}

void CmptTransform::SetTransform(const transformf & transform) {
	this->transform = transform;
	position = transform.decompose_position();
	rotation = transform.decompose_quatenion();
	scale = transform.decompose_scale();
	dirtyTransform = false;
}

const eulerf CmptTransform::GetRotationEuler() const {
	return GetTransform().decompose_euler();
}
