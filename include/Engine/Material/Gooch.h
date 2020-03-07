#pragma once

#include <Engine/Material/Material.h>

#include <Basic/Image.h>

namespace Ubpa {
	class Gooch : public Material {
	public:
		Gooch(const rgbf& colorFactor = rgbf(1.0f)) :colorFactor(colorFactor) {}
		virtual ~Gooch() = default;

	public:
		static Ptr<Gooch> New(const rgbf& colorFactor = rgbf(1.0f)) {
			return Ubpa::New<Gooch>(colorFactor);
		}

	public:
		rgbf colorFactor;
		Ptr<Image> colorTexture;
	};
}
