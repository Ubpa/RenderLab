#ifndef _CPPUTIL_ENGINE_SPOT_LIGHT_H_
#define _CPPUTIL_ENGINE_SPOT_LIGHT_H_

#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		// 朝 -y 方向照射
		class SpotLight : public Light {
		public:
			SpotLight(const RGBf &color = RGBf(1), float intensity = 1.0f, float radius = 10.f,
				float angle = 90.f, float fullRatio = 0.8f)
				: color(color), intensity(intensity), radius(radius),
				angle(angle), fullRatio(fullRatio) { }

		public:
			static const Basic::Ptr<SpotLight> New(const RGBf &color = RGBf(1), float intensity = 1.0f, float radius = 10.f,
				float angle = 90.f, float fullRatio = 0.8f) {
				return Basic::New<SpotLight>(color, intensity, radius, angle, fullRatio);
			}

		protected:
			virtual ~SpotLight() = default;

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
			virtual float PDF(const Point3 & p, const Normalf & wi) const override { return 0; }

			virtual bool IsDelta() const override { return true; }

		public:
			float CosHalfAngle() const{
				return cos(Basic::Math::Radians(angle) / 2);
			}
			float CosFalloffAngle() const {
				return cos(Basic::Math::Radians(angle) / 2 * fullRatio);
			}

		private:
			static float Fwin(float d, float radius);
			float Falloff(const Normalf & wi) const;

		public:
			RGBf color;
			float intensity;
			float radius;

			float angle; // [1 - 179]
			float fullRatio; // [0 -- 1]
		};
	}
}

#endif//!_CPPUTIL_ENGINE_SPOT_LIGHT_H_
