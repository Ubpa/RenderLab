#pragma once

#include <Engine/Scene/Component.h>

#include <UGM/transform.h>

namespace Ubpa {
	class Ray;

	class CmptCamera final : public Component {
	public:
		CmptCamera(Ptr<SObj> sobj, float fov = 60.f, float ar = 16.f / 9.f, float nearPlane = 0.001f, float farPlane = 1000.f);

	public:
		static const Ptr<CmptCamera> New(Ptr<SObj> sobj, float fov = 60.f, float ar = 16.f / 9.f, float nearPlane = 0.001f, float farPlane = 1000.f) {
			return Ubpa::New<CmptCamera>(sobj, fov, ar, nearPlane, farPlane);
		}

	protected:
		virtual ~CmptCamera() = default;

	public:
		float GetAspectRatio() const { return ar; }
		void SetAspectRatio(float ar);
		template<typename numT>
		void SetAspectRatioWH(numT w, numT h) { SetAspectRatio(static_cast<float>(w) / static_cast<float>(h)); }

		bool InitCoordinate();

		// right, up, front are normalized vector
		// !!! need call InitCoordinate() first !!!
		const Ray GenRay(float u, float v) const;

		float GetFOV() const { return fov; }
		void SetFOV(float fov);

	public:
		float nearPlane;
		float farPlane;

	private:
		// field of view
		float fov;

		// aspect ratio
		float ar;

		float w;
		float h;

		struct Coordinate {
			void Init(const transformf& tsfm);

			pointf3 pos;
			normalf right;
			normalf up;
			normalf front;
		};
		Coordinate coordinate;
	};
}
