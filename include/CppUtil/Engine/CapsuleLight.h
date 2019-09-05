#ifndef _CPPUTIL_ENGINE_LIGHT_CAPSULE_LIGHT_H_
#define _CPPUTIL_ENGINE_LIGHT_CAPSULE_LIGHT_H_

#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		class CapsuleLight : public Light {
		public:
			CapsuleLight(const RGBf & color = 1.f, float intensity = 1.f, float radius = 1.f, float height = 2.f)
				: color(color), intensity(intensity), radius(radius), height(height) { }

		public:
			static const Basic::Ptr<CapsuleLight> New(const RGBf & color = 1.f, float intensity = 1.f, float radius = 1.f, float height = 2.f) {
				return Basic::New<CapsuleLight>(color, intensity, radius, height);
			}

		public:
			virtual ~CapsuleLight() = default;

		public:
			float Area() const { return 4 * Basic::Math::PI * radius * radius + 2 * Basic::Math::PI * radius * height; }
			const RGBf LuminancePower() const { return intensity * color; }
			const RGBf Luminance() const { return LuminancePower() / (Area() * Basic::Math::PI); }

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
			float radius;
			float height;
		};
	}
}


#endif // !_CPPUTIL_ENGINE_LIGHT_CAPSULE_LIGHT_H_
