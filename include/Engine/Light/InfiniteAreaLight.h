#pragma once

#include <Engine/Light/Light.h>
#include <Basic/Sampler/AliasMethod.h>

namespace Ubpa {
	class Image;

	class InfiniteAreaLight : public Light {
	public:
		InfiniteAreaLight(Ptr<Image> img, const rgbf& colorFactor = rgbf(1.f), float intensity = 1.f)
			: colorFactor(colorFactor), intensity(intensity) {
			SetImg(img);
		}

	public:
		static const Ptr<InfiniteAreaLight> New(Ptr<Image> img, const rgbf& colorFactor = rgbf(1.f), float intensity = 1.f) {
			return Ubpa::New<InfiniteAreaLight>(img, colorFactor, intensity);
		}

	public:
		const PtrC<Image> GetImg() const { return img; }
		void SetImg(Ptr<Image> img);

	public:
		// 采样 L 函数
		// !!! p，wi 处于光源的坐标空间中
		// @arg0  in，以 p 点来计算 distToLight 和 PD
		// @arg1 out，wi 指向光源，为单位向量
		// @arg2 out，p 点到光源采样点的距离
		// @arg3 out，概率密度 probability density
		virtual const rgbf Sample_L(const pointf3& p, normalf& wi, float& distToLight, float& PD) const override;

		// 概率密度函数
		// !!! p，wi 处于灯的坐标空间中
		virtual float PDF(const pointf3& p, const normalf& wi) const override;

		virtual bool IsDelta() const override { return false; }

		// 对那些没有击中任何物体的射线调用
		virtual const rgbf Le(const Ray& ray) const override;

	private:
		const rgbf GetColor(const pointf2& texcoord) const;

	public:
		rgbf colorFactor;
		float intensity;

	private:
		Ptr<Image> img;
		AliasMethod aliasMethod;
	};
}
