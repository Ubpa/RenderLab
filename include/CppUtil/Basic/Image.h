#ifndef _BASIC_IMAGE_IMAGE_H_
#define _BASIC_IMAGE_IMAGE_H_

#include <string>
#include <CppUtil/Basic/HeapObj.h>
#include <glm/glm.hpp>

namespace CppUtil {
	namespace Basic {
		typedef unsigned char uByte;
		class Image : public HeapObj{
			HEAP_OBJ_SETUP_SELF_DEL(Image)

		public:
			Image();
			Image(size_t width, size_t height, size_t channel);
			Image(const char * fileName, bool flip = false);
			//------------
			bool IsValid() const;
			uByte * GetData();
			const uByte * GetConstData() const;
			size_t GetWidth() const;
			size_t GetHeight() const;
			size_t GetChannel() const;
			//------------
			template<glm::length_t N>
			void SetPixel(size_t x, size_t y, const glm::vec<N, float> & pixel) {
				SetPixel(x, y, Pixel_F2UB(pixel));
			}
			template<glm::length_t N>
			void SetPixel(size_t x, size_t y, const glm::vec<N, double> & pixel) {
				SetPixel(x, y, Pixel_D2UB(pixel));
			}
			template<glm::length_t N>
			void SetPixel(size_t x, size_t y, const glm::vec<N, uByte> pixel) {
				assert(channel <= N);
				for (int i = 0; i < channel; i++)
					At(x, y, i) = pixel[i];
			}

			glm::vec<4, uByte> GetPixel_UB(size_t x, size_t y) const;
			glm::vec4 GetPixel_F(size_t x, size_t y) const;
			glm::dvec4 GetPixel_D(size_t x, size_t y) const;
			uByte & At(size_t x, size_t y, size_t channel);
			const uByte & At(size_t x, size_t y, size_t channel) const;
			glm::vec4 Sample(float u, float v, bool blend = false) const;
			glm::vec4 Sample(glm::vec2 texcoord, bool blend = false) const {
				return Sample(texcoord.x, texcoord.y, blend);
			}

			//------------

			bool Load(const std::string & fileName, bool flip = false);
			void GenBuffer(size_t width, size_t height, size_t channel);
			void Free();
			bool SaveAsPNG(const std::string & fileName, bool flip = false) const;

			//------------

			template<glm::length_t N>
			static glm::vec<N, uByte> Pixel_F2UB(const glm::vec<N, float> & pixel) {
				glm::vec<N, uByte> rst;
				for (size_t i = 0; i < N; i++)
					rst[i] = static_cast<uByte>(clamp<float>(255.99f * pixel[i], 0.0f, 255.99f));

				return rst;
			}

			template<glm::length_t N>
			static glm::vec<N, uByte> Pixel_D2UB(const glm::vec<N, double> & pixel) {
				glm::vec<N, uByte> rst;
				for (size_t i = 0; i < N; i++)
					rst[i] = static_cast<uByte>(clamp<double>(255.99 * pixel[i], 0.0, 255.99));

				return rst;
			}

			template<glm::length_t N>
			static glm::vec<N, float> Pixel_UB2F(const glm::vec<N, uByte> & pixel) {
				return 1.0f / 255.0f * glm::vec<N, float>(pixel);
			}

			template<glm::length_t N>
			static glm::vec<N, double> Pixel_UB2D(const glm::vec<N, uByte> & pixel) {
				return 1.0 / 255.0 * glm::vec<N, double>(pixel);
			}
			//------------
			Image & operator =(const Image & img);
			Image(const Image & img);

		protected:
			~Image();
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
	}
}

#endif // !_IMAGE_H_
