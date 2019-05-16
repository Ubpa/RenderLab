#include <CppUtil/Basic/UGM/Transform.h>
#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/Error.h>
#include <CppUtil/Basic/UGM/Mat3x3.h>

#include <cmath>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Basic::Math;

void Transform::Init(const Point3 & pos, const Scalef & scale, const Quatf & rot) {
	(*this) = Translate(pos) * Scale(scale) * Rotate(rot);
}

const Point3 Transform::operator()(const Point3 & p) const {
	float x = p.x, y = p.y, z = p.z;
	float xp = m(0, 0) * x + m(0, 1) * y + m(0, 2) * z + m(0, 3);
	float yp = m(1, 0) * x + m(1, 1) * y + m(1, 2) * z + m(1, 3);
	float zp = m(2, 0) * x + m(2, 1) * y + m(2, 2) * z + m(2, 3);
	float wp = m(3, 0) * x + m(3, 1) * y + m(3, 2) * z + m(3, 3);
	if (wp == 1)
		return Point3(xp, yp, zp);
	else
		return Point3(xp / wp, yp / wp, zp / wp);
}

const Vec3 Transform::operator()(const Vec3 & v) const {
	float x = v.x, y = v.y, z = v.z;
	return Vec3(
		m(0, 0) * x + m(0, 1) * y + m(0, 2) * z,
		m(1, 0) * x + m(1, 1) * y + m(1, 2) * z,
		m(2, 0) * x + m(2, 1) * y + m(2, 2) * z
	);
}

const Normalf Transform::operator()(const Normalf & n) const {
	float x = n.x, y = n.y, z = n.z;
	return Normalf(
		mInv(0, 0) * x + mInv(1, 0) * y + mInv(2, 0) * z,
		mInv(0, 1) * x + mInv(1, 1) * y + mInv(2, 1) * z,
		mInv(0, 2) * x + mInv(1, 2) * y + mInv(2, 2) * z
	);
}

const BBoxf Transform::operator()(const BBoxf & box) const {
	const Transform &M = *this;
	BBoxf curBox;
	for (int i = 0; i < 8; i++)
		curBox.UnionWith(M(box.Corner(i)));
	return curBox;
}

const Ray Transform::operator()(const Ray & ray) const {
	const Transform &M = *this;

	const auto o = M(ray.o);
	const auto d = M(ray.d);

	return Ray(o, d);
}

Point3 & Transform::ApplyTo(Point3 & p) const {
	float x = p.x, y = p.y, z = p.z;
	p.x = m(0, 0) * x + m(0, 1) * y + m(0, 2) * z + m(0, 3);
	p.y = m(1, 0) * x + m(1, 1) * y + m(1, 2) * z + m(1, 3);
	p.z = m(2, 0) * x + m(2, 1) * y + m(2, 2) * z + m(2, 3);
	float wp = m(3, 0) * x + m(3, 1) * y + m(3, 2) * z + m(3, 3);

	if (wp != 1) {
		p.x /= wp;
		p.y /= wp;
		p.z /= wp;
	}

	return p;
}

Vec3 & Transform::ApplyTo(Vec3 & v) const {
	float x = v.x, y = v.y, z = v.z;
	v.x = m(0, 0) * x + m(0, 1) * y + m(0, 2) * z;
	v.y = m(1, 0) * x + m(1, 1) * y + m(1, 2) * z;
	v.z = m(2, 0) * x + m(2, 1) * y + m(2, 2) * z;
	return v;
}

Normalf & Transform::ApplyTo(Normalf & n) const {
	float x = n.x, y = n.y, z = n.z;

	n.x = mInv(0, 0) * x + mInv(1, 0) * y + mInv(2, 0) * z;
	n.y = mInv(0, 1) * x + mInv(1, 1) * y + mInv(2, 1) * z;
	n.z = mInv(0, 2) * x + mInv(1, 2) * y + mInv(2, 2) * z;
	
	return n;
}

BBoxf & Transform::ApplyTo(BBoxf & box) const {
	return box = (*this)(box);
}

Ray & Transform::ApplyTo(Ray & ray) const {
	const Transform &M = *this;

	ray.o = M(ray.o);
	ray.d = M(ray.d);

	return ray;
}

const Transform Transform::Translate(const float x, const float y, const float z) {
	Mat4f m(
		1, 0, 0, x,
		0, 1, 0, y,
		0, 0, 1, z,
		0, 0, 0, 1
	);

	Mat4f minv(
		1, 0, 0, -x,
		0, 1, 0, -y,
		0, 0, 1, -z,
		0, 0, 0, 1
	);

	return Transform(m, minv);
}

const Transform Transform::Scale(const float x, const float y, const float z) {
	return Transform(Mat4f(x, y, z, 1.f), Mat4f(1.f / x, 1.f / y, 1.f / z, 1.f));
}

const Transform Transform::RotateX(const float theta) {
	float sinTheta = std::sin(Radians(theta));
	float cosTheta = std::cos(Radians(theta));
	Mat4f m(
		1, 0, 0, 0,
		0, cosTheta, -sinTheta, 0,
		0, sinTheta, cosTheta, 0,
		0, 0, 0, 1);
	return Transform(m, m.Transpose());
}

const Transform Transform::RotateY(const float theta) {
	float sinTheta = std::sin(Radians(theta));
	float cosTheta = std::cos(Radians(theta));
	Mat4f m(cosTheta, 0, sinTheta, 0,
		0, 1, 0, 0,
		-sinTheta, 0, cosTheta, 0,
		0, 0, 0, 1);
	return Transform(m, m.Transpose());
}

const Transform Transform::RotateZ(const float theta) {
	float sinTheta = std::sin(Radians(theta));
	float cosTheta = std::cos(Radians(theta));
	Mat4f m(cosTheta, -sinTheta, 0, 0,
		sinTheta, cosTheta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	return Transform(m, m.Transpose());
}

const Transform Transform::Rotate(const Vec3 &axis, const float theta) {
	Vec3 a = axis.Normalize();
	float sinTheta = std::sin(Radians(theta));
	float cosTheta = std::cos(Radians(theta));
	Mat4f m;
	// Compute rotation of first basis vector
	m(0, 0) = a.x * a.x * (1 - cosTheta) + cosTheta;
	m(0, 1) = a.x * a.y * (1 - cosTheta) - a.z * sinTheta;
	m(0, 2) = a.x * a.z * (1 - cosTheta) + a.y * sinTheta;
	m(0, 3) = 0;

	// Compute rotations of second and third basis vectors
	m(1, 0) = a.x * a.y * (1 - cosTheta) + a.z * sinTheta;
	m(1, 1) = a.y * a.y * (1 - cosTheta) + cosTheta;
	m(1, 2) = a.y * a.z * (1 - cosTheta) - a.x * sinTheta;
	m(1, 3) = 0;

	m(2, 0) = a.x * a.z * (1 - cosTheta) - a.y * sinTheta;
	m(2, 1) = a.y * a.z * (1 - cosTheta) + a.x * sinTheta;
	m(2, 2) = a.z * a.z * (1 - cosTheta) + cosTheta;
	m(2, 3) = 0;
	return Transform(m, m.Transpose());
}

const Transform Transform::Rotate(const Quatf & q) {
	if (q.IsIdentity())
		return Transform(1.f);
	else
		return Rotate(q.GetAxis(), q.GetTheta());
}

const Transform Transform::LookAt(const Point3 &pos, const Point3 &target, const Vec3 &up) {
	const auto & ERROR = ErrorRetVal(LookAt);

	// R
	// [ right  ]
	// [ camUp  ]
	// [ -front ]
	//
	// T
	// [ pos.x ]
	// [ pos.y ]
	// [ pos.z ]
	//
	// worldToCamera
	// [ R -RT ]
	// [ 0   1 ]
	//
	// cameraToWorld
	// [ R^T  T ]
	// [ 0    1 ]


	const Vec3 front = (target - pos).Normalize();
	Vec3 right = front.Cross(up);
	if (right.IsZero()) {
		Vec3 newUp = up.Normalize();
		newUp.MinComponent() = 1;
		right = front.Cross(newUp).Normalize();
	}
	const Vec3 camUp = right.Cross(front);

	Mat4f worldToCamera;
	worldToCamera(0, 0) = right.x;
	worldToCamera(0, 1) = right.y;
	worldToCamera(0, 2) = right.z;

	worldToCamera(1, 0) = camUp.x;
	worldToCamera(1, 1) = camUp.y;
	worldToCamera(1, 2) = camUp.z;

	worldToCamera(2, 0) = -front.x;
	worldToCamera(2, 1) = -front.y;
	worldToCamera(2, 2) = -front.z;

	worldToCamera(0, 3) = -right.Dot(pos);
	worldToCamera(1, 3) = -camUp.Dot(pos);
	worldToCamera(2, 3) = front.Dot(pos);

	Mat4f cameraToWorld;
	cameraToWorld(0, 0) = right.x;
	cameraToWorld(1, 0) = right.y;
	cameraToWorld(2, 0) = right.z;

	cameraToWorld(0, 1) = camUp.x;
	cameraToWorld(1, 1) = camUp.y;
	cameraToWorld(2, 1) = camUp.z;

	cameraToWorld(0, 2) = -front.x;
	cameraToWorld(1, 2) = -front.y;
	cameraToWorld(2, 2) = -front.z;

	cameraToWorld(0, 3) = pos.x;
	cameraToWorld(1, 3) = pos.y;
	cameraToWorld(2, 3) = pos.z;

	return Transform(worldToCamera, cameraToWorld);
}

const Transform Transform::Perspcetive(const float fovy, const float aspect, const float zNear, const float zFar) {
	const float tanHalfFovy = tan(Radians(fovy) / 2.f);

	Mat4f rst(0.f);
	rst(0,0) = 1.f / (aspect * tanHalfFovy);
	rst(1,1) = 1.f / (tanHalfFovy);
	rst(2,2) = -(zFar + zNear) / (zFar - zNear);
	rst(3,2) = -1.f;
	rst(2,3) = -(2.f * zFar * zNear) / (zFar - zNear);

	return Transform(rst);
}

const Transform Transform::Orthographic(const float width, const float height, const float zNear, const float zFar) {
	Mat4f rst(1.f);

	rst(0, 0) = 2.f / width;
	rst(1, 1) = 2.f / height;
	rst(2, 2) = -2.f / (zFar - zNear);
	rst(2, 3) = -(zFar + zNear) / (zFar - zNear);

	return Transform(rst);
}

const Transform::PosRotScale Transform::Decompose() const {
	PosRotScale rst;

	rst.pos = m.GetCol(3);

	Mat3f tmpMat3(m);
	rst.scale.x = tmpMat3.GetCol(0).Norm();
	rst.scale.y = tmpMat3.GetCol(1).Norm();
	rst.scale.z = tmpMat3.GetCol(2).Norm();

	tmpMat3.GetCol(0) /= rst.scale.x;
	tmpMat3.GetCol(1) /= rst.scale.y;
	tmpMat3.GetCol(2) /= rst.scale.z;

	rst.rot.real = sqrt(tmpMat3.Tr() + 1.f) / 2.f;
	rst.rot.imag.x = tmpMat3(2, 1) - tmpMat3(1, 2);
	rst.rot.imag.y = tmpMat3(0, 2) - tmpMat3(2, 0);
	rst.rot.imag.z = tmpMat3(1, 0) - tmpMat3(0, 1);
	rst.rot.imag /= 4.f * rst.rot.w;

	return rst;
}

const EulerYXZf Transform::RotationEulerYXZ() const {
	Mat3f tmpMat3(m);
	tmpMat3.GetCol(0).NormalizeSelf();
	tmpMat3.GetCol(1).NormalizeSelf();
	tmpMat3.GetCol(2).NormalizeSelf();

	if (tmpMat3(1, 2) == -1.f) {
		const float x = 90;
		const float y = Degrees(atan2(-tmpMat3(2, 0), tmpMat3(0, 0)));
		const float z = 0;
		return EulerYXZf(x, y, z);
	}
	else if (tmpMat3(1, 2) == 1.f) {
		const float x = -90;
		const float y = Degrees(atan2(-tmpMat3(2, 0), tmpMat3(0, 0)));
		const float z = 0;
		return EulerYXZf(x, y, z);
	}
	else {
		const float x = Degrees(asin(-tmpMat3(1, 2)));
		const float y = Degrees(atan2(tmpMat3(0, 2), tmpMat3(2, 2)));
		const float z = Degrees(atan2(tmpMat3(1, 0), tmpMat3(1, 1)));
		return EulerYXZf(x, y, z);
	}
}
