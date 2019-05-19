#ifndef _CPPUTIL_ENGINE_INFINITE_AREA_LIGHT_H_
#define _CPPUTIL_ENGINE_INFINITE_AREA_LIGHT_H_

#include <CppUtil/Engine/Light.h>
#include <CppUtil/Basic/AliasMethod.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class InfiniteAreaLight : public Light {
		public:
			InfiniteAreaLight(Basic::Ptr<Basic::Image> img, const RGBf & colorFactor = RGBf(1.f), float intensity = 1.f)
				: colorFactor(colorFactor), intensity(intensity) { SetImg(img); }

		public:
			static const Basic::Ptr<InfiniteAreaLight> New(Basic::Ptr<Basic::Image> img, const RGBf & colorFactor = RGBf(1.f), float intensity = 1.f) {
				return Basic::New<InfiniteAreaLight>(img, colorFactor, intensity);
			}

		public:
			const Basic::PtrC<Basic::Image> GetImg() const { return img; }
			void SetImg(Basic::Ptr<Basic::Image> img);

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
			virtual const RGBf GetMaxL(const Point3 & p) const override { return RGBf(0.f); }

			// 直接返回最大光照
			virtual const RGBf GetMaxL() const override { return RGBf(0.f); }

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const override;

			virtual bool IsDelta() const override { return false; }

			// 对那些没有击中任何物体的射线调用
			virtual const RGBf Le(const ERay & ray) const override;

		private:
			const RGBf GetColor(const Point2 & texcoord) const;

		public:
			RGBf colorFactor;
			float intensity;

		private:
			Basic::Ptr<Basic::Image> img;
			Basic::AliasMethod aliasMethod;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_INFINITE_AREA_LIGHT_H_
