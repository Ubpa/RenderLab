#include <CppUtil/Engine/CmptTransform.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

void CmptTransform::Init(const Point3 & pos, const Vec3 & scale, const Quatf & rot) {
	position = pos;
	this->scale = scale;
	rotation = rot;
	dirtyTransform = true;
}

const Transform & CmptTransform::GetTransform() const{
	UpdateMat();
	return transform;
}

void CmptTransform::SetPosition(const Point3 & position) {
	dirtyTransform = true;
	this->position = position;
}

void CmptTransform::SetRotation(const Quatf & rotation) {
	dirtyTransform = true;
	this->rotation = rotation;
}

void CmptTransform::SetScale(const Vec3 & scale) {
	dirtyTransform = true;
	this->scale = scale;
}

void CmptTransform::UpdateMat() const{
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

const EulerYXZf CmptTransform::GetRotationEuler() const {
	return GetTransform().RotationEulerYXZ();
}
