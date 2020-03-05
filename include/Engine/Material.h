#pragma once

#include <Basic/Element.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>
#include <UGM/point.h>
#include <UGM/rgb.h>
#include <Basic/Math.h>

namespace Ubpa {
	class Image;

	class Material : public Element {
	protected:
		Material() = default;
		virtual ~Material() = default;
	};
}
