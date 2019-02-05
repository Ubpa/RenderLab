#include <CppUtil/Engine/Transform.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace glm;

Transform::Transform(Basic::Ptr<SObj> sobj)
	: Component(sobj), mat(1), inv(1), normMat(1), dirtyMat(false), dirtyInv(false), dirtyNormMat(false), position(vec3(0)), rotation(1, 0, 0, 0), scale(vec3(1))  { }

void Transform::SetPosition(const vec3 & position) {
	SetDirty();
	this->position = position;
}

void Transform::SetRotation(const quat & rotation) {
	SetDirty();
	this->rotation = rotation;
}

void Transform::SetScale(const vec3 & scale) {
	SetDirty();
	this->scale = scale;
}

void Transform::UpdateMat() {
	if (dirtyMat) {
		mat = mat4(1.0f);
		// TRS
		mat = translate(mat, position);
		mat = mat * mat4_cast(rotation);
		mat = glm::scale(mat, scale);
		dirtyMat = false;
	}
}

void Transform::UpdateInv() {
	UpdateMat();
	if (dirtyInv) {
		inv = inverse(mat);
		dirtyInv = false;
	}
}

void Transform::UpdateNormMat() {
	UpdateMat();
	if (dirtyNormMat) {
		normMat = transpose(inverse(mat3(mat)));
		dirtyNormMat = false;
	}
}

void Transform::Rotate(float radian, const glm::vec3 & axis) {
	SetDirty();
	rotation = rotate(rotation, radian, axis);
}

void Transform::Translate(const glm::vec3 & translation) {
	SetDirty();
	position += translation;
}
