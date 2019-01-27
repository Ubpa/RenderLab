#ifndef _RAY_CAMERA_H_
#define _RAY_CAMERA_H_

#include <CppUtil/Basic/HeapObj.h>
#include <glm/glm.hpp>

namespace RTX {
	class Ray;

	class RayCamera : public CppUtil::Basic::HeapObj{
		HEAP_OBJ_SETUP(RayCamera)
	public:
		// fov is top to bottom in degrees
		RayCamera(const glm::vec3 & pos, const glm::vec3 & viewPoint, float ratioWH,
			float fov = 45.0f, float lenR = 0.0f, float focus_dist = -1.0f,
			const glm::vec3 & worldUp = glm::vec3(0, 1, 0));

		virtual CppUtil::Basic::Ptr<Ray> GenRay(float s, float t) const;
		


		const glm::vec3 GetPos() const { return pos; }
		const glm::vec3 GetBL_Corner() const { return BL_Corner; }
		const glm::vec3 GetHorizontal() const { return horizontal; }
		const glm::vec3 GetVertical() const { return vertical; }
		const glm::vec3 GetRight() const { return right; }
		const glm::vec3 GetUp() const { return up; }
		const glm::vec3 GetFront() const { return front; }
		float GetLenR() const { return lenR; }
	protected:
		void GenRay(float s, float t, CppUtil::Basic::Ptr<Ray> ray) const;

		glm::vec3 pos;
		glm::vec3 BL_Corner;
		glm::vec3 horizontal;
		glm::vec3 vertical;
		glm::vec3 right, up, front;
		float lenR;

		static const float PI;
	};
}

#endif // !_RAY_CAMERA_H_
