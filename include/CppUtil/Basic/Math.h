#ifndef _MATH_H_
#define _MATH_H_

#include <glm/glm.hpp>
#include <vector>

namespace CppUtil {
	namespace Basic {
		namespace Math {
			const float EPSILON = 10e-6f;
			const float PI = 3.1415926f;

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
			glm::vec2 Sphere2UV(const glm::vec3 & normal);

			// 返回值: (alpah, beta, gamma, t)
			// e + t * d == alpha * a + beta * b + gamma * c
			// alpha + beta + gammma == 1
			// 如果平行, 返回 (0, 0, 0, 0)
			glm::vec4 Intersect_RayTri(const glm::vec3 & e, const glm::vec3 & d, const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c);

			template<typename T>
			T min(const std::vector<T> & val);

			template<typename T>
			T max(const std::vector<T> & val);

			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			//--------------以下为模板的实现----------------
			template <typename T>
			T Mean(const std::vector<T> & data) {
				if (data.size() == 0)
					return static_cast<T>(0);

				T sum = static_cast<T>(0);
				for (size_t i = 0; i < data.size(); i++)
					sum += data[i];

				return sum / data.size();
			}

			template<typename T>
			T Variance(const std::vector<T> & data) {
				if (data.size() <= 1)
					return static_cast<T>(0);

				T mean = Mean(data);
				T sum = static_cast<T>(0);
				for (size_t i = 0; i < data.size(); i++)
					sum += pow(data[i] - mean, 2);

				return sum / (data.size() - 1);
			}

			template<typename T>
			void Permute(std::vector<T> data) {
				for (size_t i = data.size() - 1; i > 0; i--) {
					size_t target = Rand_UI() % i;
					std::swap(data[i], data[target]);
				}
			}

			template<typename T>
			T min(const std::vector<T> & val) {
				if (val.empty())
					return static_cast<T>(0);

				T rst = val[0];
				for (size_t i = 1; i < val.size(); i++)
					rst = glm::min(rst, val[i]);

				return rst;
			}

			template<typename T>
			T max(const std::vector<T> & val) {
				if (val.empty())
					return static_cast<T>(0);

				T rst = val[0];
				for (size_t i = 1; i < val.size(); i++)
					rst = glm::max(rst, val[i]);

				return rst;
			}
		}
	}
}

#endif // !_MATH_H_
