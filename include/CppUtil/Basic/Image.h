#ifndef _BASIC_IMAGE_IMAGE_H_
#define _BASIC_IMAGE_IMAGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/RGBA.h>
#include <CppUtil/Basic/UGM/Point2.h>

#include <string>

namespace CppUtil {
	namespace Basic {
		class Image : public HeapObj {
		public:
			Image();
			Image(int width, int height, int channel);
			Image(const std::string & path, bool flip = false);
			Image(const Image & img);
			Image(Image && img);

		public:
			static const Ptr<Image> New() { return CppUtil::Basic::New<Image>(); }
			static const Ptr<Image> New(int width, int height, int channel) { return CppUtil::Basic::New<Image>(width, height, channel); }
			static const Ptr<Image> New(const std::string & path, bool flip = false) { return CppUtil::Basic::New<Image>(path, flip); }
			static const Ptr<Image> New(const Image & img) { return CppUtil::Basic::New<Image>(img); }
			static const Ptr<Image> New(Image && img) { return CppUtil::Basic::New<Image>(std::move(img)); }
			
		public:
			bool Load(const std::string & fileName, bool flip = false);
			void GenBuffer(int width, int height, int channel);
			void Free() noexcept;
			bool SaveAsPNG(const std::string & fileName, bool flip = false) const;

			Ptr<Image> GenFlip() const;
			Ptr<Image> Clear(const RGBAf & clearColor = RGBAf{0.f});
			Ptr<Image> Inverse();

			Image & operator =(const Image & img) noexcept;
			Image & operator =(Image && img) noexcept;

		public:
			bool IsValid() const;
			float * GetData() & { return data; }
			const float * GetData() const & { return data; }
			int GetWidth() const { return width; }
			int GetHeight() const { return height; }
			int GetChannel() const { return channel; }
			int GetPixelNum() const { return width * height; }
			int GetValNum() const { return width * height*channel; }
			const std::string & GetPath() const { return path; }

		public:
			int xy2idx(int x, int y) const;
			int xy2idx(const Point2i & xy) const {
				return xy2idx(xy.x, xy.y);
			}
			const Point2i idx2xy(int idx) const;

		public:
			const RGBAf GetPixel(int x, int y) const;
			const RGBAf GetPixel(const Point2i & xy) const { return GetPixel(xy.x, xy.y); }
			const RGBAf GetPixel(int idx) const { return GetPixel(idx2xy(idx)); }

			float & At(int x, int y, int channel);
			const float & At(int x, int y, int channel) const { return const_cast<Image *>(this)->At(x, y, channel); }
			float & At(const Point2i & xy, int channel) { return At(xy.x, xy.y, channel); }
			const float & At(const Point2i & xy, int channel) const { return const_cast<Image *>(this)->At(xy, channel); }
			float & At(int idx, int channel) { return At(idx2xy(idx), channel); }
			const float & At(int idx, int channel) const { return const_cast<Image *>(this)->At(idx, channel); }

			void SetPixel(int x, int y, float r, float g, float b);
			void SetPixel(int x, int y, const RGBf & rgb) {
				SetPixel(x, y, rgb.r, rgb.g, rgb.b);
			}
			void SetPixel(const Point2i & xy, float r, float g, float b) {
				SetPixel(xy.x, xy.y, r, g, b);
			}
			void SetPixel(const Point2i & xy, const RGBf & rgb) {
				SetPixel(xy.x, xy.y, rgb.r, rgb.g, rgb.b);
			}

			void SetPixel(int x, int y, float r, float g, float b, float a);
			void SetPixel(int x, int y, const RGBAf & rgba) {
				SetPixel(x, y, rgba.r, rgba.g, rgba.b, rgba.a);
			}
			void SetPixel(const Point2i & xy, float r, float g, float b, float a) {
				SetPixel(xy.x, xy.y, r, g, b, a);
			}
			void SetPixel(const Point2i & xy, const RGBAf & rgba) {
				SetPixel(xy.x, xy.y, rgba.r, rgba.g, rgba.b, rgba.a);
			}
			void SetPixel(const Point2i & xy, const RGBf & rgb, float a) {
				SetPixel(xy.x, xy.y, rgb.r, rgb.g, rgb.b, a);
			}
			void SetPixel(int x, int y, const RGBf & rgb, float a) {
				SetPixel(x, y, rgb.r, rgb.g, rgb.b, a);
			}

			const RGBAf SampleNearest(float u, float v) const;
			const RGBAf SampleNearest(const Point2 & texcoord) const {
				return SampleNearest(texcoord.x, texcoord.y);
			}
			const RGBAf SampleBilinear(float u, float v) const;
			const RGBAf SampleBilinear(const Point2 & texcoord) const {
				return SampleBilinear(texcoord.x, texcoord.y);
			}
			enum class Mode {
				NEAREST,
				BILINEAR,
			};
			const RGBAf Sample(float u, float v, Mode mode) const {
				switch (mode)
				{
				case Mode::NEAREST:
					return SampleNearest(u, v);
					break;
				case Mode::BILINEAR:
					return SampleBilinear(u, v);
					break;
				default:
					return RGBAf(0.f);
				}
			}
			const RGBAf Sample(const Point2 & texcoord, Mode mode) const {
				return Sample(texcoord.x, texcoord.y, mode);
			}

		protected:
			virtual ~Image() noexcept;

		private:
			float * data;
			int width;
			int height;
			int channel;
			std::string path;
		};
	}
}

#endif // !_IMAGE_H_
