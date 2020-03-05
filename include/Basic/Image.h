#pragma once

#include <Basic/HeapObj.h>
#include <UGM/rgb.h>
#include <UGM/rgba.h>
#include <UGM/point.h>
#include <UGM/val.h>

#include <string>

namespace Ubpa {
	class Image : public HeapObj {
	public:
		Image();
		Image(int width, int height, int channel);
		Image(const std::string& path, bool flip = false);
		Image(const Image& img);
		Image(Image&& img);

	public:
		static const Ptr<Image> New() { return Ubpa::New<Image>(); }
		static const Ptr<Image> New(int width, int height, int channel) { return Ubpa::New<Image>(width, height, channel); }
		static const Ptr<Image> New(const std::string& path, bool flip = false) { return Ubpa::New<Image>(path, flip); }
		static const Ptr<Image> New(const Image& img) { return Ubpa::New<Image>(img); }
		static const Ptr<Image> New(Image&& img) { return Ubpa::New<Image>(std::move(img)); }

	public:
		bool Load(const std::string& fileName, bool flip = false);
		void GenBuffer(int width, int height, int channel);
		void Free() noexcept;
		bool SaveAsPNG(const std::string& fileName, bool flip = false) const;

		Ptr<Image> GenFlip() const;
		Ptr<Image> Clear(const rgbaf& clearColor = rgbaf{ 0.f,0.f,0.f,0.f });
		Ptr<Image> inverse();

		Image& operator =(const Image& img) noexcept;
		Image& operator =(Image&& img) noexcept;

	public:
		bool IsValid() const;
		float* GetData()& { return data; }
		const float* GetData() const& { return data; }
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		int GetChannel() const { return channel; }
		int GetPixelNum() const { return width * height; }
		int GetValNum() const { return width * height * channel; }
		const std::string& GetPath() const { return path; }

	public:
		int xy2idx(int x, int y) const;
		int xy2idx(const vali2& xy) const {
			return xy2idx(xy[0], xy[1]);
		}
		const vali2 idx2xy(int idx) const;

	public:
		const rgbaf GetPixel(int x, int y) const;
		const rgbaf GetPixel(const vali2& xy) const { return GetPixel(xy[0], xy[1]); }
		const rgbaf GetPixel(int idx) const { return GetPixel(idx2xy(idx)); }

		float& At(int x, int y, int channel);
		const float& At(int x, int y, int channel) const { return const_cast<Image*>(this)->At(x, y, channel); }
		float& At(const vali2& xy, int channel) { return At(xy[0], xy[1], channel); }
		const float& At(const vali2& xy, int channel) const { return const_cast<Image*>(this)->At(xy, channel); }
		float& At(int idx, int channel) { return At(idx2xy(idx), channel); }
		const float& At(int idx, int channel) const { return const_cast<Image*>(this)->At(idx, channel); }

		void SetPixel(int x, int y, float r, float g, float b);
		void SetPixel(int x, int y, const rgbf& rgb) {
			SetPixel(x, y, rgb[0], rgb[1], rgb[2]);
		}
		void SetPixel(const vali2& xy, float r, float g, float b) {
			SetPixel(xy[0], xy[1], r, g, b);
		}
		void SetPixel(const vali2& xy, const rgbf& rgb) {
			SetPixel(xy[0], xy[1], rgb[0], rgb[1], rgb[2]);
		}

		void SetPixel(int x, int y, float r, float g, float b, float a);
		void SetPixel(int x, int y, const rgbaf& rgba) {
			SetPixel(x, y, rgba[0], rgba[1], rgba[2], rgba[3]);
		}
		void SetPixel(const vali2& xy, float r, float g, float b, float a) {
			SetPixel(xy[0], xy[1], r, g, b, a);
		}
		void SetPixel(const vali2& xy, const rgbaf& rgba) {
			SetPixel(xy[0], xy[1], rgba[0], rgba[1], rgba[2], rgba[3]);
		}
		void SetPixel(const vali2& xy, const rgbf& rgb, float a) {
			SetPixel(xy[0], xy[1], rgb[0], rgb[1], rgb[2], a);
		}
		void SetPixel(int x, int y, const rgbf& rgb, float a) {
			SetPixel(x, y, rgb[0], rgb[1], rgb[2], a);
		}

		const rgbaf SampleNearest(float u, float v) const;
		const rgbaf SampleNearest(const pointf2& texcoord) const {
			return SampleNearest(texcoord[0], texcoord[1]);
		}
		const rgbaf SampleBilinear(float u, float v) const;
		const rgbaf SampleBilinear(const pointf2& texcoord) const {
			return SampleBilinear(texcoord[0], texcoord[1]);
		}
		enum class Mode {
			NEAREST,
			BILINEAR,
		};
		const rgbaf Sample(float u, float v, Mode mode) const {
			switch (mode)
			{
			case Mode::NEAREST:
				return SampleNearest(u, v);
				break;
			case Mode::BILINEAR:
				return SampleBilinear(u, v);
				break;
			default:
				return rgbaf(0.f, 0.f, 0.f, 0.f);
			}
		}
		const rgbaf Sample(const pointf2& texcoord, Mode mode) const {
			return Sample(texcoord[0], texcoord[1], mode);
		}

	protected:
		virtual ~Image() noexcept;

	private:
		float* data;
		int width;
		int height;
		int channel;
		std::string path;
	};
}
