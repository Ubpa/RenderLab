#ifndef _ENGINE_LIGHT_AREA_LIGHT_H_
#define _ENGINE_LIGHT_AREA_LIGHT_H_

#include <CppUtil/Engine/LightBase.h>
#include <CppUtil/Basic/UniformGridSampler2D.h>

namespace CppUtil {
	namespace Engine {
		class AreaLight : public LightBase {
			ELE_SETUP(AreaLight)
		public:
			AreaLight(const glm::vec3 &color = glm::vec3(1), float intensity = 1, float width = 1, float height = 1)
				: color(color), intensity(intensity), width(width), height(height) { }

			// 采样 L 函数
			// !!! p，wi 处于灯的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 起点为灯，为单位向量
			// @arg2 out，p 点到灯光采样点的距离
			// @arg3 out，概率密度 probability density
			virtual glm::vec3 Sample_L(const glm::vec3 & p, glm::vec3 & wi, float & distToLight, float & pd) const;

			// 获取 L
			// 如果不能击中光源，则返回 vec3(0)
			virtual glm::vec3 GetL(const glm::vec3& p, const glm::vec3 & dirToLight, float & distToLight);

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const glm::vec3& p, const glm::vec3& wi) const;

			virtual bool IsDelta() const { return false; }

		public:
			glm::vec3 color;
			float intensity;
			float width;
			float height;

		private:
			bool Hit(const glm::vec3& p, const glm::vec3 & dirToLight) const;
			bool Hit(const glm::vec3& p, const glm::vec3 & dirToLight, glm::vec3 & hitPos) const;

			Basic::UniformGridSampler2D sampler;
		};
	}
}

#endif//!_ENGINE_LIGHT_AREA_LIGHT_H_
