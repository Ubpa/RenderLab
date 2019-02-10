#ifndef _ENGINE_COMPONENT_TRANSFORM_H_
#define _ENGINE_COMPONENT_TRANSFORM_H_

#include <CppUtil/Engine/Component.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace CppUtil {
	namespace Engine {

		class Transform : public Component {
			COMPONENT_SETUP(Transform)
		public:
			Transform(Basic::Ptr<SObj> sobj);

			const glm::vec3 & GetPosition() const { return position; }
			const glm::quat & GetRotation() const { return rotation; }
			const glm::vec3 & GetScale() const { return scale; }
			const glm::mat4 & GetMat() {
				UpdateMat();
				return mat;
			}
			const glm::mat4 & GetInv() {
				UpdateInv();
				return inv;
			}
			const glm::mat3 & GetNormMat() {
				UpdateNormMat();
				return normMat;
			}

			void SetPosition(const glm::vec3 & position);
			void SetRotation(const glm::quat & rotation);
			void SetScale(const glm::vec3 & scale);

			void Rotate(float radian, const glm::vec3 & axis);
			void Translate(const glm::vec3 & translation);

			void SetDirty() { dirtyMat = true; dirtyInv = true; dirtyNormMat = true; }

		private:
			void UpdateMat();
			void UpdateInv();
			void UpdateNormMat();

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			bool dirtyMat;
			bool dirtyInv;
			bool dirtyNormMat;
			glm::mat4 mat;
			glm::mat4 inv;
			glm::mat3 normMat;
		};
	}
}

#endif // !_ENGINE_COMPONENT_TRANSFORM_H_
