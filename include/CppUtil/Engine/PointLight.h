#ifndef _CPPUTIL_ENGINE_POINT_LIGHT_H_
#define _CPPUTIL_ENGINE_POINT_LIGHT_H_

#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		class PointLight : public Light<PointLight> {
		public:
			PointLight(const RGBf &color = RGBf(1), float intensity = 1.0f, float linear = 0.7f, float quadratic = 1.8f)
				: color(color), intensity(intensity), linear(linear), quadratic(quadratic) { }

		public:
			static const Basic::Ptr<PointLight> New(const RGBf &color = RGBf(1), float intensity = 1.0f, float linear = 0.7f, float quadratic = 1.8f) {
				return Basic::New<PointLight>(color, intensity, linear, quadratic);
			}

		protected:
			virtual ~PointLight() = default;

		public:
			// 采样 L 函数
			// !!! p，wi 处于灯的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 起点为灯，为单位向量
			// @arg2 out，p 点到灯光采样点的距离
			// @arg3 out，概率密度 probability density
			virtual const RGBf Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const override;

			// 获取 L
			// 如果不能击中光源，则返回 0
			virtual const RGBf GetL(const Point3 & p, const Vec3 & dirToLight, float & distToLight) override { return RGBf(0.f); }

			// 获取 L
			// 如果 p 不可能被照射到，则返回 vec3(0)
			virtual const RGBf GetMaxL(const Point3 & p) const override { return color * intensity; };

			// 直接返回最大光照
			virtual const RGBf GetMaxL() const override { return color * intensity; }

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const override { return 0; }

			virtual bool IsDelta() const override { return true; }

		public:
			RGBf color;
			float intensity;

			/*
				距离		常数项	一次项	二次项
				7		1.0		0.7		1.8
				13		1.0		0.35	0.44
				20		1.0		0.22	0.20
				32		1.0		0.14	0.07
				50		1.0		0.09	0.032
				65		1.0		0.07	0.017
				100		1.0		0.045	0.0075
				160		1.0		0.027	0.0028
				200		1.0		0.022	0.0019
				325		1.0		0.014	0.0007
				600		1.0		0.007	0.0002
				3250	1.0		0.0014	0.000007
			*/

			float linear;
			float quadratic;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_POINT_LIGHT_H_
