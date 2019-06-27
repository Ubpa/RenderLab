#ifndef _ENGINE_LIGHT_AREA_LIGHT_H_
#define _ENGINE_LIGHT_AREA_LIGHT_H_

#include <CppUtil/Engine/Light.h>
#include <CppUtil/Basic/UniformGridSampler2D.h>

namespace CppUtil {
	namespace Engine {
		// 朝上
		class AreaLight : public Light {
		public:
			AreaLight(const RGBf &color = RGBf(1), float intensity = 1, float width = 1, float height = 1)
				: color(color), intensity(intensity), width(width), height(height) { }

		public:
			static const Basic::Ptr<AreaLight> New(const RGBf &color = RGBf(1), float intensity = 1, float width = 1, float height = 1) {
				return Basic::New<AreaLight>(color, intensity, width, height);
			}

		public:
			// 采样 L 函数
			// !!! p，wi 处于光源的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 指向光源，为单位向量
			// @arg2 out，p 点到光源采样点的距离
			// @arg3 out，概率密度 probability density
			virtual const RGBf Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const override;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const override;

			virtual bool IsDelta() const override { return false; }

		public:
			RGBf color;
			float intensity;
			float width;
			float height;

		private:
			// dirToLight 不必是 单位向量
			bool Hit(const Point3 & p, const Vec3 & dirToLight, Point3 & hitPos) const;

			Basic::UniformGridSampler2D sampler;
		};
	}
}

#endif//!_ENGINE_LIGHT_AREA_LIGHT_H_
