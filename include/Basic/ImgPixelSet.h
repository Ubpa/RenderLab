#ifndef _BASIC_IMAGE_IMG_PIXEL_SET_H_
#define _BASIC_IMAGE_IMG_PIXEL_SET_H_

#include <Basic/RandSet.h>
#include <Basic/UGM/Point2.h>

namespace CppUtil {
	namespace Basic {
		class ImgPixelSet : public RandSet<CppUtil::Point2ui>{
		public:
			ImgPixelSet();
			ImgPixelSet(size_t width, size_t height);

			using RandSet<CppUtil::Point2ui>::RandPick;
			std::vector<CppUtil::Point2ui> RandPick(size_t n);
			std::vector<CppUtil::Point2ui> PickAll();
		};
	}
}

#endif // !_IMG_PIXEL_SET_H_
