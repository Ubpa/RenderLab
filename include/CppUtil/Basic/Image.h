#ifndef _BASIC_IMAGE_IMAGE_H_
#define _BASIC_IMAGE_IMAGE_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/RGB.h>
#include <CppUtil/Basic/RGBA.h>
#include <CppUtil/Basic/Point2.h>

#include <string>

namespace CppUtil {
	namespace Basic {
		class Image : public HeapObj{
			HEAP_OBJ_SETUP_SELF_DEL(Image)

		public:
			Image();
			Image(int width, int height, int channel);
			Image(const std::string & path, bool flip = false);
			Image(const Image & img);
			
		public:
			bool Load(const std::string & fileName, bool flip = false);
			void GenBuffer(int width, int height, int channel);
			void Free();
			bool SaveAsPNG(const std::string & fileName, bool flip = false) const;
			Image::Ptr GenFlip() const;

			Image & operator =(const Image & img);

		public:
			bool IsValid() const;
			float * GetData() { return data; }
			const float * GetData() const { return data; }
			int GetWidth() const { return width; }
			int GetHeight() const { return height; }
			int GetChannel() const { return channel; }
			int GetValNum() const { return width * height*channel; }
			const std::string & GetPath() const { return path; }

		public:
			const RGBAf GetPixel(int x, int y) const;
			const RGBAf GetPixel(const Point2i & xy) const {
				return GetPixel(xy.x, xy.y);
			}

			float & At(int x, int y, int channel);
			float & At(const Point2i & xy, int channel) {
				return At(xy.x, xy.y, channel);
			}
			float At(int x, int y, int channel) const;
			float At(const Point2i & xy, int channel) const {
				return At(xy.x, xy.y, channel);
			}

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
			enum Mode {
				NEAREST,
				BILINEAR,
			};
			const RGBAf Sample(float u, float v, Mode mode) const {
				switch (mode)
				{
				case CppUtil::Basic::Image::NEAREST:
					return SampleNearest(u, v);
					break;
				case CppUtil::Basic::Image::BILINEAR:
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
			~Image();
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
