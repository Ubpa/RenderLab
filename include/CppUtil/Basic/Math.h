#ifndef _BASIC_MATH_MATH_H_
#define _BASIC_MATH_MATH_H_

#include <glm/glm.hpp>
#include <vector>

namespace CppUtil {
	namespace Basic {
		namespace Math {
			const float EPSILON = 1e-6f;
			const float PI = 3.14159265358979323f;
			const float INV_PI = 0.318309886183790672f;

			glm::vec2 RandInCircle();

			glm::vec3 RandInSphere();

			// [-0x7FFFFFFFF, 0x7FFFFFFF]
			int Rand_I();

			// [0, 0xFFFFFFFF]
			unsigned int Rand_UI();

			// [0.0f, 1.0f]
			float Rand_F();

			// [0.0, 1.0]
			double Rand_D();

			// dor( (0.299, 0.587, 0.114), color )
			float Gray(const glm::vec3 & color);

			// 折射
			// rationNiNt 折射率之比 == Ni / Nt
			//     Ni 为 入射光线所在介质的折射率
			//     Nt 为 折射光线所在介质的折射率
			bool Refract(const glm::vec3 & viewDir, const glm::vec3 & normal, float ratioNiNt, glm::vec3 & refractDir);

			// 反射率
			// viewDir 为 视线方向(从物点到视点的方向)
			// halfway 为 视线方向与光线方向的平均
			// rationNtNi 折射率之比 == Nt / Ni
			//     Ni 为 入射光线所在介质的折射率
			//     Nt 为 折射光线所在介质的折射率
			float FresnelSchlick(const glm::vec3 & viewDir, const glm::vec3 & halfway, float ratioNtNi);

			template <typename T>
			T Mean(const std::vector<T> & data);

			template<typename T>
			T Variance(const std::vector<T> & data);

			// 变更所有元素的位置, 新位置的概率平均
			template<typename T>
			void Permute(std::vector<T> data);

			// 将三维坐标转化为参数坐标
			glm::vec2 SphereNormal2Texcoord(const glm::vec3 & normal);
			// 从球的法向计算切向量
			glm::vec3 SphereNormal2Tangent(const glm::vec3 & normal);

			// 返回值: (alpah, beta, gamma, t)
			// e + t * d == alpha * a + beta * b + gamma * c
			// alpha + beta + gammma == 1
			// 如果平行, 返回 (0, 0, 0, 0)
			glm::vec4 Intersect_RayTri(const glm::vec3 & e, const glm::vec3 & d, const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c);

			template<typename T>
			T min(const std::vector<T> & val);

			template<typename T>
			T max(const std::vector<T> & val);

			bool IsBase2(int n);

			// return object to world 3x3 matrix
			// n 是单位向量
			glm::mat3 GenCoordSpace(const glm::vec3 & n);

			float Illum(const glm::vec3 & color);

#include <CppUtil/Basic/Math.inl>
		}
	}
}

#endif // !_BASIC_MATH_MATH_H_
