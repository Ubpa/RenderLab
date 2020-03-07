#pragma once

#include "Primitive.h"

namespace Ubpa {
	class Disk : public Primitive {
	public:
		Disk() = default;

	public:
		static const Ptr<Disk> New() {
			return Ubpa::New<Disk>();
		}

	protected:
		virtual ~Disk() = default;

	public:
		virtual const bboxf3 GetBBox() const override {
			return { {-1,-0.001,-1}, {1,0.001,1} };
		}
	};
}
