#ifndef _ENGINE_LIGHT_LIGHT_H_
#define _ENGINE_LIGHT_LIGHT_H_

#include <CppUtil/Basic/Element.h>

#include <CppUtil/Basic/RGB.h>
#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Vector.h>
#include <CppUtil/Basic/Normal.h>

namespace CppUtil {
	namespace Engine {
		class Light : public Basic::Element {
			ELE_SETUP(Light)
		public:

			// 采样 L 函数
			// !!! p，wi 处于灯的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 起点为灯，为单位向量
			// @arg2 out，p 点到灯光采样点的距离
			// @arg3 out，概率密度 probability density
			virtual RGBf Sample_L(const Pointf & p, Normalf & wi, float & distToLight, float & PD) const = 0;

			// 获取 L
			// 如果不能击中光源，则返回 0
			virtual RGBf GetL(const Pointf & p, const Vectorf & dirToLight, float & distToLight) = 0;

			// 获取 L
			// 如果 p 不可能被照射到，则返回 vec3(0)
			virtual RGBf GetMaxL(const Pointf & p) const = 0;

			// 直接返回最大光照
			virtual RGBf GetMaxL() const = 0;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Pointf & p, const Normalf & wi) const = 0;
			
			virtual bool IsDelta() const = 0;
		};
	}
}

#endif//!_ENGINE_LIGHT_LIGHT_H_
