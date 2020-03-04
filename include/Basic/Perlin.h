#ifndef _BASIC_MATH_PERLIN_H_
#define _BASIC_MATH_PERLIN_H_

#include <UGM/vec.h>

#include <vector>

namespace CppUtil {
	namespace Basic {
		namespace Math {
			class Perlin {
			public:
				static float Noise(const Ubpa::vecf3 & p);
				static float Turb(const Ubpa::vecf3 & p, size_t depth = 7);
			private:
				static float PerlinInterp(const Ubpa::vecf3 c[2][2][2], float u, float v, float w);
				static std::vector<size_t> GenPermute(size_t n);
				static std::vector<Ubpa::vecf3> GenRandVec(size_t n);

				static std::vector<Ubpa::vecf3> randVec;

				// 0, 1, ... , 255 变更顺序后的序列
				static std::vector<size_t> permuteX;
				static std::vector<size_t> permuteY;
				static std::vector<size_t> permuteZ;
			};
		}
	}
}

#endif//!_BASIC_MATH_PERLIN_H_
