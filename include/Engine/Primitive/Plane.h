#pragma once

#include "Primitive.h"

namespace Ubpa {
	class Plane final : public Primitive {
	public:
		Plane() = default;
	public:
		static const Ptr<Plane> New() { return Ubpa::New<Plane>(); }

	protected:
		virtual ~Plane() = default;

	public:
		// primitive 局部坐标系内的 bbox
		virtual const bboxf3 GetBBox() const override {
			return bboxf3(pointf3(-0.5f, -0.001f, -0.5f), pointf3(0.5f, 0.001f, 0.5f));
		}
	};
}
