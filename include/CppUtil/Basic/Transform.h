#ifndef _CPPUTIL_BASIC_MATH_TRANSFORM_H_
#define _CPPUTIL_BASIC_MATH_TRANSFORM_H_

#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Normal.h>
#include <CppUtil/Basic/Matrix4x4.h>
#include <CppUtil/Basic/BBox.h>
#include <CppUtil/Basic/Ray.h>

namespace CppUtil {
	namespace Basic {

		// 右手系
		class Transform {
		public:
			Transform(float d = 1.f) : m(d), mInv(1.f/d) { }
			Transform(const float mat[4][4]) : m(Mat4f(mat)) {
				mInv = m.Inverse();
			}
			Transform(const Mat4f & m) : m(m), mInv(m.Inverse()) { }
			Transform(const Mat4f & m, const Mat4f & mInv) : m(m), mInv(mInv) {}

		public:
			const Mat4f & GetMatrix() const { return m; }
			const Mat4f & GetInverseMatrix() const { return mInv; }

		public:
			Transform Inverse() const {
				return Transform(mInv, m);
			}

			Transform Transpose() const {
				return Transform(m.Transpose(), mInv.Transpose());
			}

			bool operator==(const Transform & rhs)const {
				return m == rhs.m && mInv == rhs.mInv;
			}

			bool operator!=(const Transform & rhs)const {
				return m != rhs.m || mInv != rhs.mInv;
			}

		public:
			static const Transform Translate(const Vectorf & delta);
			static const Transform Scale(float x, float y, float z);
			static const Transform RotateX(float theta);
			static const Transform RotateY(float theta);
			static const Transform RotateZ(float theta);
			static const Transform Rotate(const Vectorf &axis, float theta);

			/*
			将世界坐标转换到相机坐标

			@param
				pos: 相机位置
				target: 观察目标点位置
				up: 上方向
			*/
			static const Transform LookAt(const Pointf &pos, const Pointf &target, const Vectorf &up = Vectorf(0, 1, 0));


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
			const Pointf operator()(const Pointf & p) const;
			const Vectorf operator()(const Vectorf & v) const;
			const Normalf operator()(const Normalf & n) const;
			const BBoxf operator()(const BBoxf & box) const;
			const Ray operator()(const Ray & ray) const;

			const Transform operator*(const Transform & rhs) const {
				return Transform(m * rhs.m, rhs.mInv * mInv);
			}

		private:
			Mat4f m;
			Mat4f mInv;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_TRANSFORM_H_
