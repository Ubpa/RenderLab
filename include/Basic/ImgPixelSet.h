#pragma once

#include <Basic/RandSet.h>
#include <UGM/val.h>

namespace Ubpa {
	class ImgPixelSet : public RandSet<valu2> {
	public:
		ImgPixelSet();
		ImgPixelSet(size_t width, size_t height);

		using RandSet<valu2>::RandPick;
		std::vector<valu2> RandPick(size_t n);
		std::vector<valu2> PickAll();
	};
}
