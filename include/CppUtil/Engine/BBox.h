#ifndef _ENGINE_PRIMITIVE_BBOX_H_
#define _ENGINE_PRIMITIVE_BBOX_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

namespace CppUtil {
	namespace Engine {
		class BBox {
		public:
			BBox(const glm::vec3 & minP = glm::vec3(0), const glm::vec3 & maxP = glm::vec3(0))
				: minP(minP), maxP(maxP) { }

			inline glm::vec3 GetExtent() const { return maxP - minP; }
			inline glm::vec3 GetCenter() const { return (minP + maxP) / 2.0f; }
			inline float GetRadius() const { return length(maxP - minP) / 2.0f; }
			float GetSurfaceArea() const{
				glm::vec3 extent = maxP - minP;
				return 2 * (extent.x * extent.y + extent.x * extent.z + extent.y * extent.z);
			}

			void Expand(const BBox & rhs) {
				minP = glm::min(minP, rhs.minP);
				maxP = glm::max(maxP, rhs.maxP);
			}

			const BBox operator +(const BBox & rhs) const {
				glm::vec3 minP = glm::min(minP, rhs.minP);
				glm::vec3 maxP = glm::max(maxP, rhs.maxP);
				return { minP, maxP };
			}

			BBox & operator +=(const BBox & rhs) {
				minP = glm::min(minP, rhs.minP);
				maxP = glm::max(maxP, rhs.maxP);
				return *this;
			}

			const BBox Transform(const glm::mat4 & matrix) const;

		public:
			glm::vec3 minP;
			glm::vec3 maxP;
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_BBOX_H_
