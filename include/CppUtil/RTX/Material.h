#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <CppUtil/RTX/MatVisitor.h>

#include <CppUtil/Basic/HeapObj.h>

#include <glm/glm.hpp>

#include <vector>

#define MATERIAL_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
public:\
virtual void Accept(MatVisitor::Ptr matVisitor) const{\
	matVisitor->Visit(CThis());\
}

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
			const glm::vec3 & normal = glm::vec3(0,0,1), float u = 0, float v = 0);

		CppUtil::Basic::Ptr<Ray> ray;
		Vertex vertex;
	};

	class Material : public CppUtil::Basic::HeapObj{
		HEAP_OBJ_SETUP(Material)
	public:
		// 返回值为 true 说明光线继续传播
		// 返回值为 false 说明光线不再传播
		virtual bool Scatter(const HitRecord & rec) const = 0;
		virtual void Accept(MatVisitor::Ptr matVisitor) const = 0;
	};
}

#endif // !_MATERIAL_H_
