#ifndef _IMG_PIXEL_SET_H_
#define _IMG_PIXEL_SET_H_

#include <CppUtil/Basic/RandSet.h>

#include <glm/glm.hpp>

namespace CppUtil {
	namespace Basic {
		class ImgPixelSet : public RandSet<glm::uvec2>{
		public:
			ImgPixelSet();
			ImgPixelSet(size_t width, size_t height);

			using RandSet<glm::uvec2>::RandPick;
			size_t RandPick(size_t n, std::vector<glm::uvec2> & pixels);
		};
	}
}

#endif // !_IMG_PIXEL_SET_H_
