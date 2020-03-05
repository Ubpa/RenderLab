#pragma once

#include <Engine/Primitive.h>
#include <UGM/point.h>
#include <UGM/normal.h>

namespace CppUtil {
	namespace Engine {
		class Sphere final : public Primitive {
		public:
			Sphere() = default;
		public:
			static const Basic::Ptr<Sphere> New() { return Basic::New<Sphere>(); }

		protected:
			virtual ~Sphere() = default;

		public:
			virtual const Ubpa::bboxf3 GetBBox() const override {
				return Ubpa::bboxf3(Ubpa::pointf3(-1.f,-1.f,-1.f), Ubpa::pointf3(1.f,1.f,1.f));
			}

		public:
			struct SphereCoord {
				SphereCoord(float theta, float phi) : theta(theta), phi(phi) {}

				SphereCoord(const Ubpa::pointf2 & texcoord)
				 : SphereCoord(texcoord[1] * Ubpa::PI<float>, texcoord[0] * 2.f * Ubpa::PI<float>) { }

				const Ubpa::normalf ToDir() const;

				float theta;
				float phi;
			};

			// 将法向转为纹理坐标
			// theta 对应 v，上 0 下 1
			// phi 对应 u，z 轴为 0，顺右手方向增大
			static const Ubpa::pointf2 TexcoordOf(const Ubpa::normalf & normal);

			// 将法向转为切向
			// u 方向
			static const Ubpa::normalf TangentOf(const Ubpa::normalf & normal);
		};
	}
}
