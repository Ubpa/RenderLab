#ifndef _ENGINE_COMPONENT_CMPT_TRANSFORM_H_
#define _ENGINE_COMPONENT_CMPT_TRANSFORM_H_

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil {
	namespace Engine {
		class CmptTransform final : public Component {
		public:
			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos = Point3(0),
				const Scalef & scale = Scalef(1.0f),
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
				const Scalef & scale,
				const Vec3 & axis,
				float theta
			) : CmptTransform(sobj, pos, scale, Quatf(axis, theta)) { }

			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos,
				const Scalef & scale,
				const EulerYXZf euler
			) : CmptTransform(sobj, pos, scale, euler.ToQuat()) { }

		public:
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos = Point3(0),
				const Scalef & scale = Scalef(1.0f),
				const Quatf & rot = Quatf())
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, rot);
			}
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos,
				const Scalef & scale,
				const Vec3 & axis,
				float theta)
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, axis, theta);
			}
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Point3 & pos,
				const Scalef & scale,
				const EulerYXZf euler)
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, euler);
			}

		protected:
			virtual ~CmptTransform() = default;

		public:
			void Init(const Point3 & pos = Point3(0), const Scalef & scale = Scalef(1.0f), const Quatf & rot = Quatf());
			void Init(const Point3 & pos, const Scalef & scale, const Vec3 & axis, float theta) {
				Init(pos, scale, Quatf(axis, theta));
			}
			void Init(const Point3 & pos, const Scalef & scale, const EulerYXZf euler) {
				Init(pos, scale, euler.ToQuat());
			}

		public:
			const Point3 & GetPosition() const { return position; }
			const Quatf & GetRotation() const { return rotation; }
			const EulerYXZf GetRotationEuler() const;
			const Scalef & GetScale() const { return scale; }
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
			void Scale(const Scalef & scale) {
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
			Scalef scale;

			mutable bool dirtyTransform;
			mutable Basic::Transform transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
