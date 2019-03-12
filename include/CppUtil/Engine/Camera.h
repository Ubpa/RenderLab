#ifndef _ENGINE_COMPONENT_CAMERA_H_
#define _ENGINE_COMPONENT_CAMERA_H_

#include <CppUtil/Engine/Component.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace CppUtil {
	namespace Engine {
		class Ray;

		class Camera : public Component {
			COMPONENT_SETUP(Camera)
		public:
			Camera(Basic::Ptr<SObj> sobj, float fov = 60.f, float ar = 16.f / 9.f, float nearPlane = 0.001f, float farPlane = 1000.f);

			float GetAspectRatio() const { return ar; }
			void SetAspectRatio(float ar);
			template<typename numT>
			void SetAspectRatio(numT w, numT h) { SetAspectRatio(static_cast<float>(w) / static_cast<float>(h)); }

			bool InitCoordinate();

			// right, up, front are normalized vector
			// !!! need call InitCoordinate() first !!!
			void SetRay(Basic::Ptr<Ray> ray, float u, float v);

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
				void Init(const glm::mat4 & matrix);

				glm::vec3 pos;
				glm::vec3 right;
				glm::vec3 up;
				glm::vec3 front;
			};
			Coordinate coordinate;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CAMERA_H_