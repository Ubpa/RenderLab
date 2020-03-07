#pragma once

#include <Engine/Scene/Component.h>
#include <UGM/transform.h>

namespace Ubpa {
	class CmptTransform final : public Component {
	public:
		CmptTransform(
			Ptr<SObj> sobj,
			const pointf3& pos = pointf3(0.f),
			const scalef3& scale = scalef3(1.0f),
			const quatf& rot = quatf::imag_real({ 0.f,0.f,0.f }, 1.f)
		) :
			Component(sobj),
			position(pos),
			scale(scale),
			rotation(rot),
			dirtyTransform(true) { }

		// theta is in radian
		CmptTransform(
			Ptr<SObj> sobj,
			const pointf3& pos,
			const scalef3& scale,
			const vecf3& axis,
			float theta
		) : CmptTransform(sobj, pos, scale, quatf(axis, theta)) { }

		CmptTransform(
			Ptr<SObj> sobj,
			const pointf3& pos,
			const scalef3& scale,
			const eulerf euler
		) : CmptTransform(sobj, pos, scale, euler.to_quat()) { }

	public:
		static const Ptr<CmptTransform> New(
			Ptr<SObj> sobj,
			const pointf3& pos = pointf3(0.f),
			const scalef3& scale = scalef3(1.0f),
			const quatf& rot = quatf::imag_real({ 0.f,0.f,0.f }, 1.f))
		{
			return Ubpa::New<CmptTransform>(sobj, pos, scale, rot);
		}
		// theta is in radian
		static const Ptr<CmptTransform> New(
			Ptr<SObj> sobj,
			const pointf3& pos,
			const scalef3& scale,
			const vecf3& axis,
			float theta)
		{
			return Ubpa::New<CmptTransform>(sobj, pos, scale, axis, theta);
		}
		static const Ptr<CmptTransform> New(
			Ptr<SObj> sobj,
			const pointf3& pos,
			const scalef3& scale,
			const eulerf euler)
		{
			return Ubpa::New<CmptTransform>(sobj, pos, scale, euler);
		}

	protected:
		virtual ~CmptTransform() = default;

	public:
		void Init(const pointf3& pos = pointf3(0.f), const scalef3& scale = scalef3(1.0f), const quatf& rot = quatf::imag_real({ 0.f,0.f,0.f }, 1.f));
		// theta is in radian
		void Init(const pointf3& pos, const scalef3& scale, const vecf3& axis, float theta) {
			Init(pos, scale, quatf(axis, theta));
		}
		void Init(const pointf3& pos, const scalef3& scale, const eulerf euler) {
			Init(pos, scale, euler.to_quat());
		}

	public:
		const pointf3& GetPosition() const { return position; }
		const quatf& GetRotation() const { return rotation; }
		const eulerf GetRotationEuler() const;
		const scalef3& GetScale() const { return scale; }
		const transformf& GetTransform() const;

	public:
		void SetPosition(const pointf3& position);
		void Translate(const vecf3& delta) {
			SetPosition(position + delta);
		}

		void SetRotation(const quatf& rotation);
		void SetRotation(const vecf3& axis, float angle) {
			SetRotation(quatf(axis, angle));
		}
		void SetRotation(const eulerf& euler) {
			SetRotation(euler.to_quat());
		}
		void Rotate(const quatf& rot) {
			SetRotation(rot * rotation);
		}
		void Rotate(const vecf3& axis, float angle) {
			Rotate(quatf(axis, angle));
		}
		void Rotate(const eulerf& euler) {
			Rotate(euler.to_quat() * rotation);
		}

		void SetScale(const scalef3& xyz);
		void Scale(const scalef3& scale) {
			SetScale(this->scale * scale);
		}

		void SetTransform(const transformf& transform);
		void look_at(const pointf3& pos, const pointf3& target, const vecf3& up = vecf3(0, 1, 0)) {
			SetTransform(transformf::look_at(pos, target, up).inverse());
		}

	private:
		void UpdateMat() const;

		pointf3 position;
		quatf rotation;
		scalef3 scale;

		mutable bool dirtyTransform;
		mutable transformf transform;
	};
}
