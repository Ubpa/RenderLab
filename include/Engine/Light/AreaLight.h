#pragma once

#include <Engine/Light/Light.h>
#include <Basic/Sampler/UniformGridSampler2D.h>

namespace Ubpa {
	// 朝上
	class AreaLight : public Light {
	public:
		AreaLight(const rgbf& color = rgbf(1.f), float intensity = 1.f, float width = 1.f, float height = 1.f)
			: color(color), intensity(intensity), width(width), height(height) { }

	public:
		static const Ptr<AreaLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.f, float width = 1.f, float height = 1.f) {
			return Ubpa::New<AreaLight>(color, intensity, width, height);
		}

	public:
		float Area() const { return width * height; }
		const rgbf LuminancePower() const { return intensity * color; }
		const rgbf Luminance() const { return LuminancePower() / (Area() * PI<float>); }

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

	public:
		rgbf color;
		float intensity;
		float width;
		float height;

	private:
		// dirToLight 不必是 单位向量
		bool Hit(const pointf3& p, const vecf3& dirToLight, pointf3& hitPos) const;

		UniformGridSampler2D sampler;
	};
}
