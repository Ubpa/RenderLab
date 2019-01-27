#ifndef _AABB_H_
#define _AABB_H_

#include <CppUtil/Basic/Ptr.h>

#include <glm/glm.hpp>

namespace RTX {
	class Ray;

	class AABB {
	public:
		AABB();
		AABB(const glm::vec3 & minP, const glm::vec3 & maxP, bool isValid = true);

		inline glm::vec3 GetMinP() const { return minP; }
		inline glm::vec3 GetMaxP() const { return maxP; }
		inline glm::vec3 GetExtent() const { return maxP - minP; }
		inline glm::vec3 GetCenter() const { return (minP + maxP) / 2.0f; }
		inline float GetRadius() const { return length(maxP - minP) / 2.0f; }
		float GetSurfaceArea() const;

		// 设置后会将 isValid 设置成 true
		void SetP(const glm::vec3 & minP, const glm::vec3 & maxP);
		inline bool IsValid() const { return isValid; }
		bool Hit(CppUtil::Basic::Ptr<Ray> ray) const;
		bool Hit(CppUtil::Basic::Ptr<Ray> ray, float & tMin, float &tMax) const;

		void Expand(const AABB & aabb);

		const AABB operator +(const AABB & aabb) const;
		AABB & operator +=(const AABB & aabb);

		static const AABB InValid;
	private:
		bool isValid;
		glm::vec3 minP;
		glm::vec3 maxP;
	};
}

#endif // !_AABB_H_

