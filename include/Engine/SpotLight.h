#pragma once

#include <Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		// 朝 -y 方向照射
		class SpotLight : public Light {
		public:
			SpotLight(const Ubpa::rgbf &color = Ubpa::rgbf(1.f), float intensity = 1.0f, float radius = 10.f,
				float angle = 90.f, float fullRatio = 0.8f)
				: color(color), intensity(intensity), radius(radius),
				angle(angle), fullRatio(fullRatio) { }

		public:
			static const Basic::Ptr<SpotLight> New(const Ubpa::rgbf &color = Ubpa::rgbf(1.f), float intensity = 1.0f, float radius = 10.f,
				float angle = 90.f, float fullRatio = 0.8f) {
				return Basic::New<SpotLight>(color, intensity, radius, angle, fullRatio);
			}

		protected:
			virtual ~SpotLight() = default;

		public:
			const Ubpa::rgbf IlluminancePower() const { return intensity * color; }

		public:
			// 采样 L 函数
			// !!! p，wi 处于光源的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 指向光源，为单位向量
			// @arg2 out，p 点到光源采样点的距离
			// @arg3 out，概率密度 probability density
			virtual const Ubpa::rgbf Sample_L(const Ubpa::pointf3 & p, Ubpa::normalf & wi, float & distToLight, float & PD) const override;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Ubpa::pointf3 & p, const Ubpa::normalf & wi) const override { return 0; }

			virtual bool IsDelta() const override { return true; }

		public:
			float CosHalfAngle() const{
				return cos(Ubpa::to_radian(angle) / 2);
			}
			float CosFalloffAngle() const {
				return cos(Ubpa::to_radian(angle) / 2 * fullRatio);
			}

		private:
			static float Fwin(float d, float radius);
			float Falloff(const Ubpa::normalf & wi) const;

		public:
			Ubpa::rgbf color;
			float intensity;
			float radius;

			float angle; // [1 - 179]
			float fullRatio; // [0 -- 1]
		};
	}
}
