#pragma once

#include "Primitive.h"

namespace Ubpa {
	// 半径为 1，中间圆柱长为 2
	class Capsule : public Primitive {
	public:
		Capsule(float height = 2.f) : height(height) {}

	public:
		static const Ptr<Capsule> New(float height = 2.f) { return Ubpa::New<Capsule>(height); }

	public:
		virtual ~Capsule() = default;

	public:
		virtual const bboxf3 GetBBox() const override {
			return bboxf3({ -1, -2, -1 }, { 1, 2, 1 });
		}

	public:
		float height;
	};
}
