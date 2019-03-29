#ifndef _ENGINE_PRIMITIVE_BBOX_H_
#define _ENGINE_PRIMITIVE_BBOX_H_

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

namespace CppUtil {
	namespace Engine {
		class BBox {
		public:
			BBox(const glm::vec3 & minP = glm::vec3(0),
				const glm::vec3 & maxP = glm::vec3(0))
				: minP(minP), maxP(maxP) { }

		public:
			inline glm::vec3 Diagonal() const { return maxP - minP; }
			inline glm::vec3 Center() const { return (minP + maxP) / 2.0f; }
			inline float Radius() const { return length(maxP - minP) / 2.0f; }

			float SurfaceArea() const {
				const glm::vec3 d = Diagonal();
				return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
			}

			float Volume() const {
				const glm::vec3 d = Diagonal();
				return d.x * d.y * d.z;
			}

			int MaxExtent() const {
				const glm::vec3 d = Diagonal();
				if (d.x > d.y && d.x > d.z)
					return 0;
				else if (d.y > d.z)
					return 1;
				else
					return 2;
			}

		public:
			void Expand(const BBox & rhs) {
				minP = glm::min(minP, rhs.minP);
				maxP = glm::max(maxP, rhs.maxP);
			}

			const BBox operator +(const BBox & rhs) const {
				const glm::vec3 minP = glm::min(minP, rhs.minP);
				const glm::vec3 maxP = glm::max(maxP, rhs.maxP);
				return { minP, maxP };
			}

			const BBox Transform(const glm::mat4 & matrix) const;

		public:
			union {
				glm::vec3 P[2];
				struct
				{
					glm::vec3 minP;
					glm::vec3 maxP;
				};
			};
		};
	}
}

#endif//!_ENGINE_PRIMITIVE_BBOX_H_
