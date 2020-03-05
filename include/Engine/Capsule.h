#pragma once

#include <Engine/Primitive.h>

namespace CppUtil {
	namespace Engine {
		// 半径为 1，中间圆柱长为 2
		class Capsule : public Primitive {
		public:
			Capsule(float height = 2.f) : height(height) {}

		public:
			static const Basic::Ptr<Capsule> New(float height = 2.f) { return Basic::New<Capsule>(height); }

		public:
			virtual ~Capsule() = default;

		public:
			virtual const Ubpa::bboxf3 GetBBox() const override {
				return Ubpa::bboxf3({ -1, -2, -1 }, { 1, 2, 1 });
			}

		public:
			float height;
		};
	}
}
