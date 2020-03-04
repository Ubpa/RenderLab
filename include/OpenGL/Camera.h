#ifndef _OPENGL_CAMERA_CAMERA_H_
#define _OPENGL_CAMERA_CAMERA_H_

#include <UGM/transform.h>

#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class Camera
		{
		public:
			enum ENUM_Movement {
				MOVE_FORWARD,
				MOVE_BACKWARD,
				MOVE_LEFT,
				MOVE_RIGHT,
				MOVE_UP,
				MOVE_DOWN
			};

			enum ENUM_Projection {
				PROJECTION_PERSEPCTIVE,
				PROJECTION_ORTHO
			};

		public:
			Camera(
				const Ubpa::pointf3 & position = Ubpa::pointf3(0.0f, 0.0f, 4.0f),
				float yaw = YAW,
				float pitch = PITCH,
				float ratioWH = RATIO_WH,
				float nearPlane = NEAR_PLANE,
				float farPlane = FAR_PLANE,
				const Ubpa::vecf3 & up = Ubpa::vecf3(0.0f, 1.0f, 0.0f),
				ENUM_Projection projectionMode = PROJECTION_MODE
			);

		public:
			Ubpa::transformf GetViewMatrix() const;
			Ubpa::transformf GetProjectionMatrix() const;

			// 返回世界坐标系下视锥体的的八个顶点
			const std::vector<Ubpa::pointf3> Corners() const;
			
			Ubpa::pointf3 & GetPos();
			Ubpa::vecf3 GetFront() const { return front; }

			const float & GetFOV() const { return fov; }
			const float & GetAspectRatio() const { return ratioWH; }
			const float & GetNearPlane() const { return nearPlane; }
			const float & GetFarPlane() const { return farPlane; }

		public:
			void ProcessKeyboard(ENUM_Movement direction, float deltaTime);
			void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
			void ProcessMouseScroll(float yoffset);

			void SetPerspective();
			void SetOrtho();

			void SetRatioWH(float w, float h) { ratioWH = w / h; }
			void SetRatioWH(float ratioWH) { this->ratioWH = ratioWH; }
			void SetFOV(float fov) { this->fov = fov; }
			void SetPose(const Ubpa::pointf3 & pos, float yaw, float pitch);

		private:
			void updateCameraVectors();

			// Camera Attributes
			Ubpa::pointf3 position;
			Ubpa::vecf3 front;
			Ubpa::vecf3 up;
			Ubpa::vecf3 right;
			Ubpa::vecf3 worldUp;
			float fov;
			float ratioWH;
			float nearPlane;
			float farPlane;
			float yaw;
			float pitch;
			ENUM_Projection projectionMode;

			// Default camera values
			static const float RATIO_WH;
			static const float NEAR_PLANE;
			static const float FAR_PLANE;
			static const float YAW;
			static const float PITCH;
			static const float FOV;
			static const ENUM_Projection PROJECTION_MODE;
		};
	}
}
#endif//! _OPENGL_CAMERA_CAMERA_H_
