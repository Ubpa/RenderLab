#ifndef _BASIC_IMAGE_IMG_PIXEL_SET_H_
#define _BASIC_IMAGE_IMG_PIXEL_SET_H_

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

#endif // !_IMG_PIXEL_SET_H_
