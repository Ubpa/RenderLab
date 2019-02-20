#include <CppUtil/Engine/Transform.h>

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

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

void Transform::Rotate(float radian, const vec3 & axis) {
	SetDirty();
	rotation = rotate(rotation, radian, axis);
	
}

void Transform::Translate(const vec3 & translation) {
	SetDirty();
	position += translation;
}

vec3 Transform::GetEulerRoatation() const {
	vec3 euler;
	extractEulerAngleYZX(mat4_cast(rotation), euler.y, euler.z, euler.x);
	return euler;
}

void Transform::SetRotation(const vec3 & eulerAngle) {
	SetRotation(quat_cast(mat3(eulerAngleYZX(eulerAngle.y, eulerAngle.z, eulerAngle.x))));
}

void Transform::LookAt(const vec3 & eye, const vec3 & center, const vec3 & up) {
	mat = inverse(lookAt(eye, center, up));
	dirtyMat = false;
	dirtyInv = true;

	position = eye;
	scale = vec3(1, 1, 1);
	rotation = quat_cast(mat);
}

void Transform::SetMatrix(const glm::mat4 & matrix) {
	mat = matrix;
	dirtyMat = false;
	dirtyInv = true;

	position[0] = matrix[3][0];
	position[1] = matrix[3][1];
	position[2] = matrix[3][2];

	mat3x3 mat33(matrix);

	scale[0] = length(mat33[0]);
	scale[1] = length(mat33[1]);
	scale[2] = length(mat33[2]);

	mat33[0] /= scale[0];
	mat33[1] /= scale[1];
	mat33[2] /= scale[2];

	rotation = quat_cast(mat33);
}
