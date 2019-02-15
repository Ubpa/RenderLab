#ifndef _ENGINE_COMPONENT_CAMERA_H_
#define _ENGINE_COMPONENT_CAMERA_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class Camera : public Component {
			COMPONENT_SETUP(Camera)
		public:
			Camera(Basic::Ptr<SObj> sobj, float fov = 60.f, float ar = 16.f / 9.f, float nearClipping = 0.001f, float farClipping = 1000.f);

			float GetAspectRatio() const { return ar; }
			void SetAspectRatio(float ar);
			void SetAspectRatio(float w, float h) { SetAspectRatio(w / h); }

			Basic::Ptr<Ray> GenRay(float u, float v);

			float GetFOV() const { return fov; }
			void SetFOV(float fov);

		private:
			// field of view
			float fov;

		private:
			// aspect ratio
			float ar;
			float nearClipping;
			float farClipping;

			float w;
			float h;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CAMERA_H_