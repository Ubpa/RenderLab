#ifndef _CPPUTIL_ENGINE_POINT_LIGHT_H_
#define _CPPUTIL_ENGINE_POINT_LIGHT_H_

#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		class PointLight : public Light {
		public:
			PointLight(const RGBf &color = 1, float intensity = 1.0f, float radius = 10.f)
				: color(color), intensity(intensity), radius(radius) { }

		public:
			static const Basic::Ptr<PointLight> New(const RGBf &color = RGBf(1), float intensity = 1.0f, float radius = 10.f) {
				return Basic::New<PointLight>(color, intensity, radius);
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

		private:
			static float Fwin(float d, float radius);

		public:
			RGBf color;
			float intensity;
			float radius;
		};
	}
}

#endif//!_CPPUTIL_ENGINE_POINT_LIGHT_H_
