#pragma once

#include <Engine/Light.h>
#include <Basic/AliasMethod.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class InfiniteAreaLight : public Light {
		public:
			InfiniteAreaLight(Basic::Ptr<Basic::Image> img, const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float intensity = 1.f)
				: colorFactor(colorFactor), intensity(intensity) { SetImg(img); }

		public:
			static const Basic::Ptr<InfiniteAreaLight> New(Basic::Ptr<Basic::Image> img, const Ubpa::rgbf & colorFactor = Ubpa::rgbf(1.f), float intensity = 1.f) {
				return Basic::New<InfiniteAreaLight>(img, colorFactor, intensity);
			}

		public:
			const Basic::PtrC<Basic::Image> GetImg() const { return img; }
			void SetImg(Basic::Ptr<Basic::Image> img);

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
			virtual float PDF(const Ubpa::pointf3 & p, const Ubpa::normalf & wi) const override;

			virtual bool IsDelta() const override { return false; }

			// 对那些没有击中任何物体的射线调用
			virtual const Ubpa::rgbf Le(const ERay & ray) const override;

		private:
			const Ubpa::rgbf GetColor(const Ubpa::pointf2 & texcoord) const;

		public:
			Ubpa::rgbf colorFactor;
			float intensity;

		private:
			Basic::Ptr<Basic::Image> img;
			Basic::AliasMethod aliasMethod;
		};
	}
}
