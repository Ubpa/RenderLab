#pragma once

#include <Engine/Light/Light.h>

namespace Ubpa {
	// 局部坐标系中，朝下 -y
	class DirectionalLight : public Light {
	public:
		DirectionalLight(const rgbf& color = rgbf(1.f), float intensity = 1.0f)
			: color(color), intensity(intensity) { }

	public:
		static const Ptr<DirectionalLight> New(const rgbf& color = rgbf(1.f), float intensity = 1.0f) {
			return Ubpa::New<DirectionalLight>(color, intensity);
		}

	protected:
		virtual ~DirectionalLight() = default;

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
		virtual float PDF(const pointf3& p, const normalf& wi) const override { return 0.f; }

		virtual bool IsDelta() const override { return true; }

	public:
		rgbf color;
		float intensity;
	};
}
