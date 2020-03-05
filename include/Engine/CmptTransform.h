#ifndef _ENGINE_COMPONENT_CMPT_TRANSFORM_H_
#define _ENGINE_COMPONENT_CMPT_TRANSFORM_H_

#include <Engine/Component.h>
#include <UGM/transform.h>

namespace CppUtil {
	namespace Engine {
		class CmptTransform final : public Component {
		public:
			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos = Ubpa::pointf3(0.f),
				const Ubpa::scalef3 & scale = Ubpa::scalef3(1.0f),
				const Ubpa::quatf & rot = Ubpa::quatf::imag_real({0.f,0.f,0.f},1.f)
			) :
				Component(sobj),
				position(pos),
				scale(scale),
				rotation(rot),
				dirtyTransform(true) { }

			// theta is in radian
			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos,
				const Ubpa::scalef3 & scale,
				const Ubpa::vecf3 & axis,
				float theta
			) : CmptTransform(sobj, pos, scale, Ubpa::quatf(axis, theta)) { }

			CmptTransform(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos,
				const Ubpa::scalef3 & scale,
				const Ubpa::eulerf euler
			) : CmptTransform(sobj, pos, scale, euler.to_quat()) { }

		public:
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos = Ubpa::pointf3(0.f),
				const Ubpa::scalef3 & scale = Ubpa::scalef3(1.0f),
				const Ubpa::quatf & rot = Ubpa::quatf::imag_real({0.f,0.f,0.f},1.f))
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, rot);
			}
			// theta is in radian
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos,
				const Ubpa::scalef3 & scale,
				const Ubpa::vecf3 & axis,
				float theta)
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, axis, theta);
			}
			static const Basic::Ptr<CmptTransform> New(
				Basic::Ptr<SObj> sobj,
				const Ubpa::pointf3 & pos,
				const Ubpa::scalef3 & scale,
				const Ubpa::eulerf euler)
			{
				return Basic::New<CmptTransform>(sobj, pos, scale, euler);
			}

		protected:
			virtual ~CmptTransform() = default;

		public:
			void Init(const Ubpa::pointf3 & pos = Ubpa::pointf3(0.f), const Ubpa::scalef3 & scale = Ubpa::scalef3(1.0f), const Ubpa::quatf & rot = Ubpa::quatf::imag_real({0.f,0.f,0.f},1.f));
			// theta is in radian
			void Init(const Ubpa::pointf3 & pos, const Ubpa::scalef3 & scale, const Ubpa::vecf3 & axis, float theta) {
				Init(pos, scale, Ubpa::quatf(axis, theta));
			}
			void Init(const Ubpa::pointf3 & pos, const Ubpa::scalef3 & scale, const Ubpa::eulerf euler) {
				Init(pos, scale, euler.to_quat());
			}

		public:
			const Ubpa::pointf3 & GetPosition() const { return position; }
			const Ubpa::quatf & GetRotation() const { return rotation; }
			const Ubpa::eulerf GetRotationEuler() const;
			const Ubpa::scalef3 & GetScale() const { return scale; }
			const Ubpa::transformf & GetTransform() const;

		public:
			void SetPosition(const Ubpa::pointf3 & position);
			void Translate(const Ubpa::vecf3 & delta) {
				SetPosition(position + delta);
			}

			void SetRotation(const Ubpa::quatf & rotation);
			void SetRotation(const Ubpa::vecf3 & axis, float angle) {
				SetRotation(Ubpa::quatf(axis, angle));
			}
			void SetRotation(const Ubpa::eulerf & euler) {
				SetRotation(euler.to_quat());
			}
			void Rotate(const Ubpa::quatf & rot) {
				SetRotation(rot * rotation);
			}
			void Rotate(const Ubpa::vecf3 & axis, float angle) {
				Rotate(Ubpa::quatf(axis, angle));
			}
			void Rotate(const Ubpa::eulerf & euler) {
				Rotate(euler.to_quat() * rotation);
			}

			void SetScale(const Ubpa::scalef3& xyz);
			void Scale(const Ubpa::scalef3 & scale) {
				SetScale(this->scale * scale);
			}

			void SetTransform(const Ubpa::transformf & transform);
			void look_at(const Ubpa::pointf3 &pos, const Ubpa::pointf3 &target, const Ubpa::vecf3 &up = Ubpa::vecf3(0, 1, 0)) {
				SetTransform(Ubpa::transformf::look_at(pos, target, up).inverse());
			}

		private:
			void UpdateMat() const;

			Ubpa::pointf3 position;
			Ubpa::quatf rotation;
			Ubpa::scalef3 scale;

			mutable bool dirtyTransform;
			mutable Ubpa::transformf transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
