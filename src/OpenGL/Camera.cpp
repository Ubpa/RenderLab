#include <OpenGL/Camera.h>

#include <Basic/Math.h>

using namespace Ubpa;

using namespace std;

const float Camera::RATIO_WH = 1.0f;
const float Camera::NEAR_PLANE = 0.01f;
const float Camera::FAR_PLANE = 100.0f;
const float Camera::YAW = -90.0f;
const float Camera::PITCH = 0.0f;
const float Camera::FOV = 50.0f;
const Camera::ENUM_Projection Camera::PROJECTION_MODE = Camera::PROJECTION_PERSEPCTIVE;

// Constructor with vectors
Camera::Camera(
	const pointf3 & position,
	float yaw,
	float pitch,
	float ratioWH,
	float nearPlane,
	float farPlane,
	const vecf3 & up,
	ENUM_Projection projectionMode
)
	:
	position(position),
	yaw(yaw),
	pitch(pitch),
	ratioWH(ratioWH),
	nearPlane(nearPlane),
	farPlane(farPlane),
	worldUp(up),
	projectionMode(projectionMode),
	fov(FOV)
{
	updateCameraVectors();
}

void Camera::SetPerspective() {
	projectionMode = ENUM_Projection::PROJECTION_PERSEPCTIVE;
}

void Camera::SetOrtho() {
	projectionMode = ENUM_Projection::PROJECTION_ORTHO;
}

pointf3 & Camera::GetPos() {
	return position;
}

// Returns the view matrix calculated using Euler Angles and the look_at Matrix
transformf Camera::GetViewMatrix() const {
	return transformf::look_at(position, (position + front).cast_to<pointf3>(), up);
}

// Returns the projection matrix calculated using zoom, ratioWH, nearPlane, farPlane
transformf Camera::GetProjectionMatrix() const {
	switch (projectionMode)
	{
	case Camera::PROJECTION_PERSEPCTIVE:
		return transformf::perspective(to_radian(fov), ratioWH, nearPlane, farPlane);
	case Camera::PROJECTION_ORTHO:
		return transformf::orthographic(fov / 2.0f, fov / 2.0f / ratioWH, nearPlane, farPlane);
	default:
		return transformf::eye();
	}
}

const vector<pointf3> Camera::Corners() const {
	auto w2n = GetProjectionMatrix() * GetViewMatrix();
	auto n2w = w2n.inverse();

	const pointf3 nCorners[8] = {
		{-1,-1,-1},
		{-1,-1, 1},
		{-1, 1,-1},
		{-1, 1, 1},
		{ 1,-1,-1},
		{ 1,-1, 1},
		{ 1, 1,-1},
		{ 1, 1, 1},
	};

	vector<pointf3> wCorners;
	for (int i = 0; i < 8; i++) {
		const auto wCorner = n2w * nCorners[i];
		wCorners.push_back(wCorner);
	}

	return wCorners;
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
	// Calculate the new Front vector
	front[0] = cos(to_radian(yaw)) * cos(to_radian(pitch));
	front[1] = sin(to_radian(pitch));
	front[2] = sin(to_radian(yaw)) * cos(to_radian(pitch));
	front.normalize_self();
	// Also re-calculate the Right and Up vector
	right = front.cross(worldUp).normalize();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = right.cross(front).normalize();
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= 0.02f;
	yoffset *= 0.02f;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	else if (fov <= 1.0f)
		fov = 1.0f;
	else if (fov >= 179.0f)
		fov = 179.0f;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(ENUM_Movement direction, float deltaTime) {
	float velocity = 5 * deltaTime;
	if (direction == MOVE_FORWARD)
		position += front * velocity;
	if (direction == MOVE_BACKWARD)
		position -= front * velocity;
	if (direction == MOVE_LEFT)
		position -= right * velocity;
	if (direction == MOVE_RIGHT)
		position += right * velocity;
	if (direction == MOVE_UP)
		position += up * velocity;
	if (direction == MOVE_DOWN)
		position -= up * velocity;
}

void Camera::SetPose(const pointf3 & pos, float yaw, float pitch) {
	position = pos;
	this->yaw = yaw;
	this->pitch = pitch;
	updateCameraVectors();
}
