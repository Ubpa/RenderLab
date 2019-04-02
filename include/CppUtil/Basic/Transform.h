#ifndef _CPPUTIL_BASIC_MATH_TRANSFORM_H_
#define _CPPUTIL_BASIC_MATH_TRANSFORM_H_

#include <CppUtil/Basic/Point3.h>
#include <CppUtil/Basic/Vector3.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Quat.h>
#include <CppUtil/Basic/EulerYXZ.h>
#include <CppUtil/Basic/Mat4x4.h>
#include <CppUtil/Basic/BBox.h>
#include <CppUtil/Basic/Ray.h>

namespace CppUtil {
	namespace Basic {

		// 右手系
		class Transform {
		public:
			template<typename T>
			Transform(T d) : m(static_cast<float>(d)), mInv(1.f/ static_cast<float>(d)) { }
			Transform() :Transform(1.f) { }
			Transform(const float mat[4][4]) : m(Mat4f(mat)), mInv(m.Inverse()) { }
			Transform(const Mat4f & m) : m(m), mInv(m.Inverse()) { }
			Transform(const Mat4f & m, const Mat4f & mInv) : m(m), mInv(mInv) {}

		public:
			const Mat4f & GetMatrix() const { return m; }
			const Mat4f & GetInverseMatrix() const { return mInv; }

		public:
			const Transform Inverse() const {
				return Transform(mInv, m);
			}

			const Transform Transpose() const {
				return Transform(m.Transpose(), mInv.Transpose());
			}

			struct PosRotScale {
				Point3 pos;
				Quatf rot;
				Vec3 scale;
			};
			const PosRotScale Decompose() const;

			const Point3 Position() const {
				return m.GetCol(3);
			}
			const Quatf RotationQuat() const {
				return Decompose().rot;
			}
			const EulerYXZf RotationEulerYXZ() const;
			const Vec3 Scale() const {
				return Vec3(m.GetCol(0).Length(), m.GetCol(1).Length(), m.GetCol(2).Length());
			}

			bool operator==(const Transform & rhs)const {
				return m == rhs.m && mInv == rhs.mInv;
			}

			bool operator!=(const Transform & rhs)const {
				return m != rhs.m || mInv != rhs.mInv;
			}

		public:
			static const Transform Translate(float x, float y, float z);
			static const Transform Translate(const Vec3 & delta) {
				return Translate(delta.x, delta.y, delta.z);
			}
			static const Transform Scale(float x, float y, float z);
			static const Transform Scale(const Vec3 & scale) {
				return Scale(scale.x, scale.y, scale.z);
			}
			static const Transform RotateX(float theta);
			static const Transform RotateY(float theta);
			static const Transform RotateZ(float theta);

			// first Y, then X, final Z
			static const Transform Rotate(const EulerYXZf & euler) {
				return RotateZ(euler.z) * RotateX(euler.x) * RotateY(euler.y);
			}
			
			static const Transform Rotate(const Vec3 &axis, float theta);
			static const Transform Rotate(const Quatf & q) {
				return Rotate(q.GetAxis(), q.GetTheta());
			}

			/*
			将世界坐标转换到相机坐标

			@param
				pos: 相机位置
				target: 观察目标点位置
				up: 上方向
			*/
			static const Transform LookAt(const Point3 &pos, const Point3 &target, const Vec3 &up = Vec3(0, 1, 0));


			/*
			正交变换，将相机坐标转换到标准坐标系 [-1, 1]^3
			*/
			static const Transform Orthographic(float width, float height, float zNear, float zFar);

			/*
			透视变换，将相机坐标转换到标准坐标系 [-1, 1]^3
			
			@param
				fovy: 纵视域，角度
				aspect: 宽高比
				zNear: 近平面 z
				zFar: 远平面 z
			*/
			static const Transform Perspcetive(float fovy, float aspect, float zNear, float zFar);

		public:
			const Point3 operator()(const Point3 & p) const;
			const Vec3 operator()(const Vec3 & v) const;
			const Normalf operator()(const Normalf & n) const;
			const BBoxf operator()(const BBoxf & box) const;
			const Ray operator()(const Ray & ray) const;

			Point3 & ApplyTo(Point3 & p) const;
			Vec3 & ApplyTo(Vec3 & v) const;
			Normalf & ApplyTo(Normalf & n) const;
			BBoxf & ApplyTo(BBoxf & box) const;
			Ray & ApplyTo(Ray & ray) const;

			const Transform operator*(const Transform & rhs) const { return Transform(m * rhs.m, rhs.mInv * mInv); }
			Transform & operator*=(const Transform & rhs) {
				m *= rhs.m;
				rhs.mInv.MulTo(mInv);
				return *this;
			}
			Transform & MulTo(Transform & rhs) const {
				m.MulTo(rhs.m);
				rhs.mInv *= mInv;
				return rhs;
			}

		private:
			Mat4f m;
			Mat4f mInv;
		};
	}

	using Transform = Basic::Transform;
}

#endif // !_CPPUTIL_BASIC_MATH_TRANSFORM_H_
