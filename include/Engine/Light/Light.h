#pragma once

#include <Engine/Viewer/Ray.h>

#include <Basic/HeapObj.h>

#include <UGM/rgb.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>

namespace Ubpa {
	class Light : public HeapObj {
	protected:
		Light() = default;
		virtual ~Light() = default;

	public:
		// 采样 L 函数
		// !!! p，wi 处于光源的坐标空间中
		// @arg0  in，以 p 点来计算 distToLight 和 PD
		// @arg1 out，wi 指向光源，为单位向量
		// @arg2 out，p 点到光源采样点的距离
		// @arg3 out，概率密度 probability density
		virtual const rgbf Sample_L(const pointf3& p, normalf& wi, float& distToLight, float& PD) const = 0;

		// 概率密度函数
		// !!! p，wi 处于灯的坐标空间中
		virtual float PDF(const pointf3& p, const normalf& wi) const = 0;

		virtual bool IsDelta() const = 0;

		// 对那些没有击中任何物体的射线调用
		virtual const rgbf Le(const Ray& ray) const { return rgbf(0.f); }
	};
}
