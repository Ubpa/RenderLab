#pragma once

#include <Basic/RandSet.h>
#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class ImgPixelSet : public RandSet<Ubpa::valu2>{
		public:
			ImgPixelSet();
			ImgPixelSet(size_t width, size_t height);

			using RandSet<Ubpa::valu2>::RandPick;
			std::vector<Ubpa::valu2> RandPick(size_t n);
			std::vector<Ubpa::valu2> PickAll();
		};
	}
}
