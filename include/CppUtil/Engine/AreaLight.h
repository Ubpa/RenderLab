#ifndef _ENGINE_LIGHT_AREA_LIGHT_H_
#define _ENGINE_LIGHT_AREA_LIGHT_H_

#include <CppUtil/Engine/Light.h>
#include <CppUtil/Basic/UniformGridSampler2D.h>

namespace CppUtil {
	namespace Engine {
		class AreaLight : public Light {
			ELE_SETUP(AreaLight)
		public:
			AreaLight(const RGBf &color = RGBf(1), float intensity = 1, float width = 1, float height = 1)
				: color(color), intensity(intensity), width(width), height(height) { }

			// 采样 L 函数
			// !!! p，wi 处于灯的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 起点为灯，为单位向量
			// @arg2 out，p 点到灯光采样点的距离
			// @arg3 out，概率密度 probability density
			virtual RGBf Sample_L(const Pointf & p, Normalf & wi, float & distToLight, float & PD) const;

			// 获取 L
			// 如果不能击中光源，则返回 0
			virtual RGBf GetL(const Pointf & p, const Vectorf & dirToLight, float & distToLight);

			// 获取 L
			// 如果 p 不可能被照射到，则返回 vec3(0)
			virtual RGBf GetMaxL(const Pointf & p) const = 0;

			// 直接返回最大光照
			virtual RGBf GetMaxL() const { return color * intensity; }

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Pointf & p, const Normalf & wi) const;

			virtual bool IsDelta() const { return false; }

		public:
			RGBf color;
			float intensity;
			float width;
			float height;

		private:
			// dirToLight 不必是 单位向量
			bool Hit(const Pointf & p, const Vectorf & dirToLight, Pointf & hitPos) const;

			Basic::UniformGridSampler2D sampler;
		};
	}
}

#endif//!_ENGINE_LIGHT_AREA_LIGHT_H_
