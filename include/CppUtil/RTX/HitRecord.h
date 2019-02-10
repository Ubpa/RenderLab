#ifndef _RTX_RT_MATERIAL_HIT_RECORD_H_
#define _RTX_RT_MATERIAL_HIT_RECORD_H_

#include <CppUtil/Basic/Ptr.h>
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>

namespace RTX {
	class Ray;

	struct Vertex {
		Vertex(glm::vec3 pos = glm::vec3(0), glm::vec3 normal = glm::vec3(0, 0, 1), float u = 0, float v = 0);
		glm::vec3 pos;
		glm::vec3 normal;
		float u;
		float v;

		void Transform(const glm::mat4 & transform);
		void Transform(const glm::mat4 & transform, const glm::mat3 & normalTransform);
		static const Vertex Interpolate_Tri(const glm::vec3 & abg, const Vertex & A, const Vertex & B, const Vertex & C);
	};

	struct HitRecord {
		HitRecord(CppUtil::Basic::Ptr<Ray> ray = NULL, const glm::vec3 & pos = glm::vec3(0),
			const glm::vec3 & normal = glm::vec3(0, 0, 1), float u = 0, float v = 0);

		CppUtil::Basic::Ptr<Ray> ray;
		Vertex vertex;
	};
}

#endif // !_RTX_RT_MATERIAL_HIT_RECORD_H_
