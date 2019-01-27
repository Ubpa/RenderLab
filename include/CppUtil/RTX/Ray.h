#ifndef _RAY_H_
#define _RAY_H_

#include <CppUtil/Basic/HeapObj.h>
#include <glm/glm.hpp>

namespace RTX {
	class Ray : public CppUtil::Basic::HeapObj{
		HEAP_OBJ_SETUP(Ray)
	public:
		Ray(const glm::vec3 & origin = glm::vec3(0), const glm::vec3 & dir = glm::vec3(1));

		inline glm::vec3 operator()(float t) const { return origin + t * dir; }
		inline glm::vec3 At(float t) const { return origin + t * dir; }

		inline glm::vec3 GetOrigin() const { return origin; };
		inline glm::vec3 GetDir() const { return dir; }
		inline glm::vec3 GetColor() const { return color; }
		inline float GetTMax() const { return tMax; }
		inline void SetTMax(float tMax) { this->tMax = tMax; }

		void Init(const glm::vec3 & origin, const glm::vec3 & dir);
		void Update(const glm::vec3 & origin, const glm::vec3 & dir, const glm::vec3 & attenuation);
		void SetLightColor(const glm::vec3 & lightColor);
		void Transform(const glm::mat4 & transform);
		
		virtual Ray::Ptr GenCopy() const;

		static const float tMin;
	protected:
		void CopyTo(Ray::Ptr ray) const;

		glm::vec3 origin;
		glm::vec3 dir;
		glm::vec3 color;
		float tMax;
	};
}
#endif //!_RAY_H_