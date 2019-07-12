#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif // WIN32

#include <CppUtil/Basic/Image.h>

#define STB_IMAGE_IMPLEMENTATION
#include <CppUtil/Basic/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <CppUtil/Basic/stb_image_write.h>

#include <CppUtil/Basic/File.h>
#include <CppUtil/Basic/Math.h>
#include <CppUtil/Basic/StrAPI.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace std;

Image::Image()
	:data(nullptr), width(0), height(0), channel(0) { }

Image::Image(int width, int height, int channel)
	: data(nullptr)
{
	GenBuffer(width, height, channel);
}

Image::Image(const string & path, bool flip)
	: data(nullptr)
{
	Load(path, flip);
}

Image::~Image() {
	Free();
}

bool Image::Load(const std::string & fileName, bool flip) {
	Free();

	stbi_set_flip_vertically_on_load(flip);

	if (StrAPI::IsEndWith(fileName, ".hdr")) {
		auto fData = stbi_loadf(fileName.c_str(), &width, &height, &channel, 0);
		if (!fData) {
			data = nullptr;
			return false;
		}

		const int valNum = width * height * channel;
		data = new float[valNum];
		for (int i = 0; i < valNum; i++)
			data[i] = fData[i];

		stbi_image_free(fData);
	}
	else {
		auto ucData = stbi_load(fileName.c_str(), &width, &height, &channel, 0);
		if (!ucData) {
			data = nullptr;
			return false;
		}

		const int valNum = width * height * channel;
		data = new float[valNum];
		for (int i = 0; i < valNum; i++)
			data[i] = static_cast<float>(ucData[i]) / 255.f;

		stbi_image_free(ucData);
	}

	path = fileName;
	return true;
}

void Image::GenBuffer(int width, int height, int channel) {
	Free();
	this->width = width;
	this->height = height;
	this->channel = channel;

	data = new float[width*height*channel]();
}

void Image::Free() noexcept {
	delete[] data;

	width = 0;
	height = 0;
	channel = 0;
	data = nullptr;
	path.clear();
}

bool Image::SaveAsPNG(const string & fileName, bool flip) const {
	if (!IsValid())
		return false;

	stbi_flip_vertically_on_write(flip);
	const int valNum = width * height*channel;
	auto ucData = new stbi_uc[valNum];
	for (int i = 0; i < valNum; i++) {
		float val255f = data[i] * 255.f;

		// clamp
		if (val255f > 255.f)
			val255f = 255.f;
		else if (val255f < 0.f)
			val255f = 0.f;

		auto val255uc = static_cast<stbi_uc>(val255f);
		if (val255f - static_cast<float>(val255uc) > 0.5f)
			val255uc += 1;

		ucData[i] = val255uc;
	}

	auto rst = stbi_write_png(fileName.c_str(), width, height, channel, ucData, width * channel);

	delete[] ucData;
	return rst;
}

Image & Image::operator=(const Image & img) noexcept {
	Free();

	width = img.width;
	height = img.height;
	channel = img.channel;

	const int valNum = width * height * channel;
	data = new float[valNum];
	memcpy(data, img.data, valNum * sizeof(float));

	return *this;
}

Image & Image::operator =(Image && img) noexcept {
	Free();

	width = img.width;
	height = img.height;
	channel = img.channel;

	data = img.data;
	img.data = nullptr;

	return *this;
}

Image::Image(const Image & img) {
	width = img.width;
	height = img.height;
	channel = img.channel;

	const int valNum = width * height * channel;
	data = new float[valNum];
	memcpy(data, img.data, valNum * sizeof(float));
}

Image::Image(Image && img) {
	width = img.width;
	height = img.height;
	channel = img.channel;
	data = img.data;

	img.data = nullptr;
}

Ptr<Image> Image::GenFlip() const {
	if (!IsValid())
		return nullptr;

	auto img = Image::New(width, height, channel);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++)
			img->SetPixel(i, height - 1 - j, GetPixel(i, j));
	}

	return img;
}

bool Image::IsValid() const{
	return data != NULL && channel > 0 && channel <= 4;
}

const RGBAf Image::GetPixel(int x, int y) const {
	RGBAf rgba(0, 0, 0, 1);
	for (int i = 0; i < channel; i++)
		rgba[i] = At(x, y, i);

	return rgba;
}

float & Image::At(int x, int y, int channel) {
	assert(channel < this->channel);
	return data[(y*width + x)*this->channel + channel];
}

void Image::SetPixel(int x, int y, float r, float g, float b) {
	assert(channel == 3);
	At(x, y, 0) = r;
	At(x, y, 1) = g;
	At(x, y, 2) = b;
}

void Image::SetPixel(int x, int y, float r, float g, float b, float a) {
	assert(channel == 4);
	At(x, y, 0) = r;
	At(x, y, 1) = g;
	At(x, y, 2) = b;
	At(x, y, 3) = a;
}

const RGBAf Image::SampleNearest(float u, float v) const {
	u = Math::Clamp(u, 0.f, 0.999999f);
	v = Math::Clamp(v, 0.f, 0.999999f);
	float xf = u * width;
	float yf = v * height;
	int xi = static_cast<int>(xf);
	int yi = static_cast<int>(yf);
	return GetPixel(xi, yi);
}

const RGBAf Image::SampleBilinear(float u, float v) const {
	float xf = Math::Clamp<float>(u, 0, 0.999999f) * width;
	float yf = Math::Clamp<float>(v, 0, 0.999999f) * height;

	int x0 = static_cast<int>(xf);
	int x1 = Math::Clamp<int>(x0 + ((xf - x0) < 0.5 ? -1 : 1), 0, width - 1);
	int y0 = static_cast<int>(yf);
	int y1 = Math::Clamp<int>(y0 + ((yf - y0) < 0.5 ? -1 : 1), 0, height - 1);

	RGBAf colors[4] = {
		GetPixel(x0,y0),
		GetPixel(x1,y0),
		GetPixel(x0,y1),
		GetPixel(x1,y1),
	};

	float tx = abs(xf - (x0 + 0.5f));
	float ty = abs(yf - (y0 + 0.5f));
	RGBAf mixColor = RGBAf::Lerp(RGBAf::Lerp(colors[0], colors[1], tx), RGBAf::Lerp(colors[2], colors[3], tx), ty);

	return mixColor;
}

Ptr<Image> Image::Clear(const RGBAf & clearColor) {
	if (!IsValid()) {
		printf("ERROR::Image::Clear:\n"
			"\t""img is invalid\n");
		return nullptr;
	}

	if (channel <= 3) {
		const auto rgb = clearColor.ToRGB();
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				for (int c = 0; c < channel; c++)
					At(x, y, c) = rgb[c];
			}
		}
	}
	else{
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++)
				SetPixel(x, y, clearColor);
		}
	}

	return This<Image>();
}

int Image::xy2idx(int x, int y) const {
	assert(x >= 0 && x < width);
	assert(y >= 0 && y < height);

	return x + y * width;
}

const Point2i Image::idx2xy(int idx) const {
	assert(idx >= 0 && idx < width * height);
	int y = idx / width;
	int x = idx - y * width;
	return { x, y };
}

Ptr<Image> Image::Inverse() {
	if (!IsValid()) {
		printf("ERROR::Image::Clear:\n"
			"\t""img is invalid\n");
		return nullptr;
	}

	int n = GetValNum();
	for (int i = 0; i < n; i++)
		data[i] = 1 - data[i];

	return This<Image>();
}
