#include <CppUtil/Engine/Transform.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace glm;

void Transform::SetPosition(const vec3 & position) {
	dirtyMat = true;
	dirtyInv = true;
	this->position = position;
}

void Transform::SetRotation(const quat & rotation) {
	dirtyMat = true;
	dirtyInv = true;
	this->rotation = rotation;
}

void Transform::SetScale(const vec3 & scale) {
	dirtyMat = true;
	dirtyInv = true;
	this->scale = scale;
}

void Transform::UpdateMat() {
	if (dirtyMat) {
		mat = mat4(1.0f);
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