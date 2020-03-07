#pragma once

#include "Primitive.h"

#include <UGM/point.h>
#include <UGM/normal.h>

namespace Ubpa {
	class Sphere final : public Primitive {
	public:
		Sphere() = default;
	public:
		static const Ptr<Sphere> New() { return Ubpa::New<Sphere>(); }

	protected:
		virtual ~Sphere() = default;

	public:
		virtual const bboxf3 GetBBox() const override {
			return bboxf3(pointf3(-1.f, -1.f, -1.f), pointf3(1.f, 1.f, 1.f));
		}

	public:
		struct SphereCoord {
			SphereCoord(float theta, float phi) : theta(theta), phi(phi) {}

			SphereCoord(const pointf2& texcoord)
				: SphereCoord(texcoord[1] * PI<float>, texcoord[0] * 2.f * PI<float>) { }

			const normalf ToDir() const;

			float theta;
			float phi;
		};

		// 将法向转为纹理坐标
		// theta 对应 v，上 0 下 1
		// phi 对应 u，z 轴为 0，顺右手方向增大
		static const pointf2 TexcoordOf(const normalf& normal);

		// 将法向转为切向
		// u 方向
		static const normalf TangentOf(const normalf& normal);
	};
}
