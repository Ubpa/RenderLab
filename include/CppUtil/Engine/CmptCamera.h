#ifndef _ENGINE_COMPONENT_CMPT_CAMERA_H_
#define _ENGINE_COMPONENT_CMPT_CAMERA_H_

#include <CppUtil/Engine/Component.h>

#include <CppUtil/Basic/Transform.h>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class CmptCamera : public Component {
			COMPONENT_SETUP(CmptCamera)
		public:
			CmptCamera(Basic::Ptr<SObj> sobj, float fov = 60.f, float ar = 16.f / 9.f, float nearPlane = 0.001f, float farPlane = 1000.f);

			float GetAspectRatio() const { return ar; }
			void SetAspectRatio(float ar);
			template<typename numT>
			void SetAspectRatioWH(numT w, numT h) { SetAspectRatio(static_cast<float>(w) / static_cast<float>(h)); }

			bool InitCoordinate();

			// right, up, front are normalized vector
			// !!! need call InitCoordinate() first !!!
			void SetRay(Ray & ray, float u, float v);

			float GetFOV() const { return fov; }
			void SetFOV(float fov);

		public:

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
				void Init(const Basic::Transform & tsfm);

				Point3 pos;
				Normalf right;
				Normalf up;
				Normalf front;
			};
			Coordinate coordinate;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_CAMERA_H_