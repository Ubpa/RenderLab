#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <glm/glm.hpp>

namespace CppUtil {
	namespace Basic {
		typedef unsigned char uByte;
		class Image {
		public:

			template<typename T>
			struct Pixel {
				Pixel(size_t channel)
					: channel(channel) { };
				Pixel(T r, T g, T b)
					: r(r), g(g), b(b), channel(3) { };
				Pixel(T r, T g, T b, T a)
					: r(r), g(g), b(b), a(a), channel(4) { };

				T & operator[](size_t idx);
				const T & operator[](size_t idx) const;

				T r;
				T g;
				T b;
				T a;
				const size_t channel;
			};

			Image();
			Image(size_t width, size_t height, size_t channel);
			Image(const char * fileName, bool flip = false);
			~Image();
			//------------
			bool IsValid() const;
			uByte * GetData();
			const uByte * GetConstData() const;
			size_t GetWidth() const;
			size_t GetHeight() const;
			size_t GetChannel() const;
			//------------
			bool SetPixel(size_t x, size_t y, const Pixel<uByte> & pixel);
			bool SetPixel(size_t x, size_t y, const Pixel<float> & pixel);
			bool SetPixel(size_t x, size_t y, const Pixel<double> & pixel);
			bool SetPixel(size_t x, size_t y, const glm::vec3 & pixel);
			Pixel<uByte> GetPixel_UB(size_t x, size_t y) const;
			Pixel<float> GetPixel_F(size_t x, size_t y) const;
			Pixel<double> GetPixel_D(size_t x, size_t y) const;
			uByte & At(size_t x, size_t y, size_t channel);
			const uByte & At(size_t x, size_t y, size_t channel) const;
			//------------
			bool Load(const std::string & fileName, bool flip = false);
			void GenBuffer(size_t width, size_t height, size_t channel);
			void Free();
			bool SaveAsPNG(const std::string & fileName, bool flip = false) const;
			//------------
			static Pixel<uByte> Pixel_F2UB(const Pixel<float> & pixel);
			static Pixel<uByte> Pixel_D2UB(const Pixel<double> & pixel);
			static Pixel<float> Pixel_UB2F(const Pixel<uByte> & pixel);
			static Pixel<double> Pixel_UB2D(const Pixel<uByte> & pixel);
			//------------
			Image & operator =(const Image & img) = delete;
		private:
			enum ENUM_SRC_TYPE
			{
				ENUM_SRC_TYPE_INVALID,
				ENUM_SRC_TYPE_NEW,
				ENUM_SRC_TYPE_STB,
			};

			uByte * data;
			size_t width;
			size_t height;
			size_t channel;
			ENUM_SRC_TYPE type;
		};


		//------------

		template<typename T>
		T & Image::Pixel<T>::operator[](size_t idx) {
			switch (idx)
			{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			case 3:
				return a;
			}
		}

		template<typename T>
		const T & Image::Pixel<T>::operator[](size_t idx) const {
			switch (idx)
			{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			case 3:
				return a;
			}
		}
	}
}

#endif // !_IMAGE_H_
