#ifndef _PERLIN_H_
#define _PERLIN_H_

#include <glm/glm.hpp>
#include <vector>

namespace CppUtil {
	namespace Basic {
		namespace Math {
			class Perlin {
			public:
				static float Noise(const glm::vec3 & p);
				static float Turb(const glm::vec3 & p, size_t depth = 7);
			private:
				static float PerlinInterp(const glm::vec3 c[2][2][2], float u, float v, float w);
				static std::vector<size_t> GenPermute(size_t n);
				static std::vector<glm::vec3> GenRandVec(size_t n);

				static std::vector<glm::vec3> randVec;

				// 0, 1, ... , 255 变更顺序后的序列
				static std::vector<size_t> permuteX;
				static std::vector<size_t> permuteY;
				static std::vector<size_t> permuteZ;
			};
		}
	}
}

#endif




