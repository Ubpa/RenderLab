#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <CppUtil/Engine/Component.h>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

namespace CppUtil {
	namespace Engine {
		class Transform : public Component {
		public:
			Transform();

			const glm::mat4 & GetMat() const { return mat; }
			void SetMat(const glm::mat4 & mat) { this->mat = mat; }
		private:
			glm::mat4 mat;
		};
	}
}

#endif // !_TRANSFORM_H_
