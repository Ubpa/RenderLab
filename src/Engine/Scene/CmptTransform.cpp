#include <Engine/CmptTransform.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

void CmptTransform::Init(const Ubpa::pointf3 & pos, const Ubpa::scalef3 & scale, const Ubpa::quatf & rot) {
	position = pos;
	this->scale = scale;
	rotation = rot;
	dirtyTransform = true;
}

const Ubpa::transformf & CmptTransform::GetTransform() const{
	UpdateMat();
	return transform;
}

void CmptTransform::SetPosition(const Ubpa::pointf3 & position) {
	dirtyTransform = true;
	this->position = position;
}

void CmptTransform::SetRotation(const Ubpa::quatf & rotation) {
	dirtyTransform = true;
	this->rotation = rotation;
}

void CmptTransform::SetScale(const Ubpa::scalef3& scale) {
	dirtyTransform = true;
	this->scale = scale;
}

void CmptTransform::UpdateMat() const{
	if (dirtyTransform) {
		const auto scaleTsfm = Ubpa::transformf(scale);
		const auto rotateTsfm = Ubpa::transformf(rotation);
		const auto translateTsfm = Ubpa::transformf(position.cast_to<Ubpa::vecf3>());
		transform = translateTsfm * rotateTsfm * scaleTsfm;

		dirtyTransform = false;
	}
}

void CmptTransform::SetTransform(const Ubpa::transformf & transform) {
	this->transform = transform;
	position = transform.decompose_position();
	rotation = transform.decompose_quatenion();
	scale = transform.decompose_scale();
	dirtyTransform = false;
}

const Ubpa::eulerf CmptTransform::GetRotationEuler() const {
	return GetTransform().decompose_euler();
}
