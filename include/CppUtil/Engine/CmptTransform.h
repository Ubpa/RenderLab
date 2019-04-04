#ifndef _ENGINE_COMPONENT_CMPT_TRANSFORM_H_
#define _ENGINE_COMPONENT_CMPT_TRANSFORM_H_

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace Engine {
		class CmptTransform : public Component {
			COMPONENT_SETUP(CmptTransform)
		public:
			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos = Point3(0),
				const Vec3 & scale = Vec3f(1.0f),
				const Quatf & rot = Quatf()
			) :
				Component(sobj),
				position(pos),
				scale(scale),
				rotation(rot),
				dirtyTransform(true) { }

			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos,
				const Vec3 & scale,
				const Vec3 & axis,
				float theta
			) : CmptTransform(sobj, pos, scale, Quatf(axis, theta)) { }

			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos,
				const Vec3 & scale,
				const EulerYXZf euler
			) : CmptTransform(sobj, pos, scale, euler.ToQuat()) { }

		public:
			void Init(const Point3 & pos = Point3(0), const Vec3 & scale = Vec3f(1.0f), const Quatf & rot = Quatf());
			void Init(const Point3 & pos, const Vec3 & scale, const Vec3 & axis, float theta) {
				Init(pos, scale, Quatf(axis, theta));
			}
			void Init(const Point3 & pos, const Vec3 & scale, const EulerYXZf euler) {
				Init(pos, scale, euler.ToQuat());
			}

		public:
			const Point3 & GetPosition() const { return position; }
			const Quatf & GetRotation() const { return rotation; }
			const EulerYXZf GetRotationEuler() const;
			const Vec3 & GetScale() const { return scale; }
			const Basic::Transform & GetTransform() const;

		public:
			void SetPosition(const Point3 & position);
			void Translate(const Vec3 & delta) {
				SetPosition(position + delta);
			}

			void SetRotation(const Quatf & rotation);
			void SetRotation(const Vec3 & axis, float angle) {
				SetRotation(Quatf(axis, angle));
			}
			void SetRotation(const EulerYXZf & euler) {
				SetRotation(euler.ToQuat());
			}
			void Rotate(const Quatf & rot) {
				SetRotation(rot * rotation);
			}
			void Rotate(const Vec3 & axis, float angle) {
				Rotate(Quatf(axis, angle));
			}
			void Rotate(const EulerYXZf & euler) {
				Rotate(euler.ToQuat() * rotation);
			}

			void SetScale(const Vec3 & xyz);
			void Scale(const Vec3 & scale) {
				SetScale(this->scale * scale);
			}

			void SetTransform(const Basic::Transform & transform);
			void LookAt(const Point3 &pos, const Point3 &target, const Vec3 &up = Vec3(0, 1, 0)) {
				SetTransform(Transform::LookAt(pos, target, up));
			}

		private:
			void UpdateMat() const;

			Point3 position;
			Quatf rotation;
			Vec3 scale;

			mutable bool dirtyTransform;
			mutable Basic::Transform transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
