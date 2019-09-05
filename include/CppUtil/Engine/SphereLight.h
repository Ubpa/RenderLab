#include <CppUtil/Engine/Light.h>

namespace CppUtil {
	namespace Engine {
		class SphereLight : public Light {
		public:
			SphereLight(const RGBf & color = 1, float intensity = 1.f, float radius = 1.f)
				: color(color), intensity(intensity), radius(radius) { }

		public:
			static Basic::Ptr<SphereLight> New(const RGBf & color = 1, float intensity = 1.f, float radius = 1.f) {
				return Basic::New<SphereLight>(color, intensity, radius);
			}

		protected:
			virtual ~SphereLight() = default;

		public:
			float Area() const { return 4 * Basic::Math::PI * radius * radius; }
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
		};
	}
}