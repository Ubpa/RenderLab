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
			template<typename numT>
			void SetAspectRatio(numT w, numT h) { SetAspectRatio(static_cast<float>(w) / static_cast<float>(h)); }

			Basic::Ptr<Ray> GenRay(float u, float v);

			float GetFOV() const { return fov; }
			void SetFOV(float fov);

		public:
			float nearClipping;
			float farClipping;

		private:
			// field of view
			float fov;

			// aspect ratio
			float ar;

			float w;
			float h;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CAMERA_H_