#ifndef _ENGINE_LIGHT_LIGHT_H_
#define _ENGINE_LIGHT_LIGHT_H_

#include <Engine/Ray.h>

#include <Basic/Element.h>

#include <UGM/rgb.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>

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
			virtual const Ubpa::rgbf Sample_L(const Ubpa::pointf3 & p, Ubpa::normalf & wi, float & distToLight, float & PD) const = 0;

			// 概率密度函数
			// !!! p，wi 处于灯的坐标空间中
			virtual float PDF(const Ubpa::pointf3 & p, const Ubpa::normalf & wi) const = 0;

			virtual bool IsDelta() const = 0;

			// 对那些没有击中任何物体的射线调用
			virtual const Ubpa::rgbf Le(const ERay & ray) const { return Ubpa::rgbf(0.f); }
		};
	}
}

#endif//!_ENGINE_LIGHT_LIGHT_H_
