#ifndef _ENGINE_LIGHT_LIGHT_H_
#define _ENGINE_LIGHT_LIGHT_H_

#include <CppUtil/Basic/Element.h>

#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/Point3.h>
#include <CppUtil/Basic/UGM/Vector3.h>
#include <CppUtil/Basic/UGM/Normal.h>

namespace CppUtil {
	namespace Engine {
		class LightBase : public Basic::ElementBase {
		protected:
			LightBase() = default;
			virtual ~LightBase() = default;

		public:
			// 采样 L 函数
			// !!! p，wi 处于灯的坐标空间中
			// @arg0  in，以 p 点来计算 distToLight 和 PD
			// @arg1 out，wi 起点为灯，为单位向量
			// @arg2 out，p 点到灯光采样点的距离
			// @arg3 out，概率密度 probability density
			virtual const RGBf Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const = 0;

			// 获取 L
			// 如果不能击中光源，则返回 0
			virtual const RGBf GetL(const Point3 & p, const Vec3 & dirToLight, float & distToLight) = 0;

			// 获取 L
			// 如果 p 不可能被照射到，则返回 vec3(0)
			virtual const RGBf GetMaxL(const Point3 & p) const = 0;

			// 直接返回最大光照
			virtual const RGBf GetMaxL() const = 0;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Point3 & p, const Normalf & wi) const = 0;

			virtual bool IsDelta() const = 0;
		};

		template<typename ImplT, typename BaseT = LightBase>
		class Light : public Basic::Element<ImplT, BaseT> {
		protected:
			using Element<ImplT, BaseT>::Element;
			virtual ~Light() = default;
		};
	}
}

#endif//!_ENGINE_LIGHT_LIGHT_H_
