#ifndef _IMG_TEXTURE_H_
#define _IMG_TEXTURE_H_

#include <CppUtil/RTX/Texture.h>

#include <CppUtil/Basic/Ptr.h>

#include <string>

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace RTX {
	class ImgTexture : public Texture{
		TEXTURE_SETUP(ImgTexture)
	public:
		typedef CppUtil::Basic::CPtr<CppUtil::Basic::Image> ImgCPtr;
		ImgTexture(const std::string & fileName, bool flip = false);

		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		bool IsValid() const;
		const ImgCPtr GetImg() const { return img; }
	private:
		ImgCPtr img;
	};
}

#endif // !_IMG_TEXTURE_H_
