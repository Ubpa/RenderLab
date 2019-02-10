#ifndef _ENGINE_COMPONENT_CAMERA_H_
#define _ENGINE_COMPONENT_CAMERA_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class Camera : public Component {
			COMPONENT_SETUP(Camera)
		public:
			Camera(Basic::Ptr<SObj> sobj, float ar = 16.f / 9.f, float fov = 60.f, float nearClipping = 0.001f, float farClipping = 1000.f);

			void SetWidthHeight(int w, int h);

			Basic::Ptr<Ray> GenRay(float u, float v);
		private:
			// aspect ratio
			float ar;
			// field of view
			float fov;

			float nearClipping;
			float farClipping;

			float w;
			float h;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CAMERA_H_