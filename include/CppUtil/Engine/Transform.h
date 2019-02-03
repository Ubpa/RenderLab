#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <CppUtil/Engine/Component.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace CppUtil {
	namespace Engine {

		class Transform : public Component {
			COMPONENT_SETUP(Transform)
		public:
			Transform(Basic::Ptr<SObj> sobj)
				: Component(sobj) { }

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

			void SetPosition(const glm::vec3 & position);
			void SetRotation(const glm::quat & rotation);
			void SetScale(const glm::vec3 & scale);

		private:
			void UpdateMat();
			void UpdateInv();

			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			bool dirtyMat;
			bool dirtyInv;
			glm::mat4 mat;
			glm::mat4 inv;
		};
	}
}

#endif // !_TRANSFORM_H_
