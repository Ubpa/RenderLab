#ifndef _ENGINE_LIGHT_LIGHT_H_
#define _ENGINE_LIGHT_LIGHT_H_

#include <Engine/Ray.h>

#include <Basic/Element.h>

#include <Basic/UGM/RGB.h>
#include <Basic/UGM/Point3.h>
#include <Basic/UGM/Vector3.h>
#include <Basic/UGM/Normal.h>

namespace CppUtil {
	namespace Engine {
		class Light : public Basic::Element {
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
			virtual const RGBf Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const = 0;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const = 0;

			virtual bool IsDelta() const = 0;

			// 对那些没有击中任何物体的射线调用
			virtual const RGBf Le(const ERay & ray) const { return RGBf(0.f); }
		};
	}
}

#endif//!_ENGINE_LIGHT_LIGHT_H_
