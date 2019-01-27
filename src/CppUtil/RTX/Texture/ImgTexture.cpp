#include <CppUtil/RTX/ImgTexture.h>

#include <CppUtil/Basic/Image.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

ImgTexture::ImgTexture(const std::string & fileName, bool flip)
	: img(new Image(fileName.c_str(), flip)) { }

rgb ImgTexture::Value(float u, float v, const glm::vec3 & p) const {
	if (!img->IsValid())
		return rgb(0);

	size_t i = clamp<float>(u*img->GetWidth(), 0, img->GetWidth() - 1);
	size_t j = clamp<float>(v*img->GetHeight(), 0, img->GetHeight() - 1);

	auto pixel = img->GetPixel_F(i, j);
	return rgb(pixel.r, pixel.g, pixel.b);
}

bool ImgTexture::IsValid() const {
	return img->IsValid();
}