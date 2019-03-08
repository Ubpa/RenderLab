#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif // WIN32

#include <CppUtil/Basic/Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <CppUtil/Basic/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <CppUtil/Basic/stb_image_write.h>

#include <CppUtil/Basic/File.h>

using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

Image::Image()
	:data(NULL), width(0), height(0), channel(0), type(ENUM_SRC_TYPE_INVALID){ }

Image::Image(int width, int height, int channel){
	GenBuffer(width, height, channel);
}

Image::Image(const char * fileName, bool flip) {
	data = NULL;
	type = ENUM_SRC_TYPE_INVALID;
	Load(fileName, flip);
}

Image::~Image() {
	Free();
}

//-----------

bool Image::IsValid() const{
	return data != NULL && type != ENUM_SRC_TYPE_INVALID;
}

//------------

uByte & Image::At(int x, int y, int channel) {
	return data[(y*width + x)*this->channel + channel];
}

const uByte & Image::At(int x, int y, int channel) const {
	return data[(y*width + x)*this->channel + channel];
}

vec4 Image::Sample(float u, float v, bool blend) const {
	// bilinear filtering

	assert(type != ENUM_SRC_TYPE_INVALID);

	float xf = clamp<float>(u, 0, 0.999999f) * width;
	float yf = clamp<float>(v, 0, 0.999999f) * height;

	int x0 = static_cast<int>(xf);
	int x1 = clamp<int>(x0 + ((xf - x0) < 0.5 ? -1 : 1), 0, width - 1);
	int y0 = static_cast<int>(yf);
	int y1 = clamp<int>(y0 + ((yf - y0) < 0.5 ? -1 : 1), 0, height - 1);

	vec4 colors[4] = {
		GetPixel_F(x0,y0),
		GetPixel_F(x1,y0),
		GetPixel_F(x0,y1),
		GetPixel_F(x1,y1),
	};

	if (blend) {
		assert(channel == 4);
		for (int i = 0; i < 4; i++) {
			colors[i].r *= colors[i].a;
			colors[i].g *= colors[i].a;
			colors[i].b *= colors[i].a;
		}
	}

	float tx = abs(xf - (x0 + 0.5f));
	float ty = abs(yf - (y0 + 0.5f));
	vec4 mixColor = (1 - ty)*((1 - tx)*colors[0] + tx * colors[1]) + ty * ((1 - tx)*colors[2] + tx * colors[3]);

	if (blend && mixColor.a != 0) {
		mixColor.r /= mixColor.a;
		mixColor.g /= mixColor.a;
		mixColor.b /= mixColor.a;
	}
	return mixColor;
}

vec<4,uByte> Image::GetPixel_UB(int x, int y) const {
	vec<4,uByte> rst(0);
	for (int i = 0; i < channel; i++)
		rst[i] = At(x, y, i);

	return rst;
}

vec4 Image::GetPixel_F(int x, int y) const {
	return Pixel_UB2F(GetPixel_UB(x, y));
}

dvec4 Image::GetPixel_D(int x, int y) const {
	return Pixel_UB2D(GetPixel_UB(x, y));
}

//------------

bool Image::Load(const std::string & fileName, bool flip) {
	Free();

	stbi_set_flip_vertically_on_load(flip);

	int tmpW, tmpH, tmpC;
	data = stbi_load(fileName.c_str(), &tmpW, &tmpH, &tmpC, 0);
	width = tmpW;
	height = tmpH;
	channel = tmpC;

	if (data == NULL) {
		type = ENUM_SRC_TYPE_INVALID;
		return false;
	}

	type = ENUM_SRC_TYPE_STB;
	path = fileName;
	return true;
}
void Image::GenBuffer(int width, int height, int channel) {
	Free();
	this->width = width;
	this->height = height;
	this->channel = channel;

	data = new uByte[width*height*channel]();
	type = ENUM_SRC_TYPE_NEW;
}

void Image::Free() {
	if (data != NULL) {
		switch (type)
		{
		case Basic::Image::ENUM_SRC_TYPE_NEW:
			delete[] data;
			break;
		case Basic::Image::ENUM_SRC_TYPE_STB:
			stbi_image_free(data);
			break;
		case Basic::Image::ENUM_SRC_TYPE_INVALID:
		default:
			break;
		}
	}

	width = 0;
	height = 0;
	channel = 0;
	data = NULL;
	path.clear();
	type = ENUM_SRC_TYPE_INVALID;
}

bool Image::SaveAsPNG(const string & fileName, bool flip) const{
	stbi_flip_vertically_on_write(flip);
	return stbi_write_png(fileName.c_str(), width, height, channel, data, width * channel);
}

Image & Image::operator =(const Image & img) {
	Free();
	width = img.width;
	height = img.height;
	channel = img.channel;
	type = ENUM_SRC_TYPE_NEW;
	int bytesNum = width * height * channel;
	data = new uByte[bytesNum];
	memcpy(data, img.data, bytesNum * sizeof(uByte));

	return *this;
}

Image::Image(const Image & img) {
	width = img.width;
	height = img.height;
	channel = img.channel;
	type = ENUM_SRC_TYPE_NEW;
	int bytesNum = width * height * channel;
	data = new uByte[bytesNum];
	memcpy(data, img.data, bytesNum * sizeof(uByte));
}

Image::Ptr Image::GenFlip() const {
	if (!IsValid())
		return nullptr;

	auto img = ToPtr(new Image(width, height, channel));

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++)
			img->SetPixel(i, height - 1 - j, GetPixel_UB(i, j));
	}

	return img;
}
