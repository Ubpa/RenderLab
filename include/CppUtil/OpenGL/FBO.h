#ifndef _FBO_H_
#define _FBO_H_

#include <CppUtil/OpenGL/Texture.h>

#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class FBO {
		public:
			enum ENUM_TYPE
			{
				ENUM_TYPE_BASIC,
				ENUM_TYPE_RGBF1_DEPTH,
				ENUM_TYPE_RGBF2_DEPTH,
				ENUM_TYPE_RGBF3_DEPTH,
				ENUM_TYPE_MSAA,
				ENUM_TYPE_COLOR,
				ENUM_TYPE_RED,
				ENUM_TYPE_COLOR_FLOAT,
				ENUM_TYPE_DEPTH,
				ENUM_TYPE_CUBE_DEPTH,
				ENUM_TYPE_GBUFFER,
				ENUM_TYPE_RAYTRACING,
				ENUM_TYPE_RTX,
			};
			enum ENUM_PASS_TYPE {
				ENUM_PASS_COLOR,
				ENUM_PASS_DEPTH,
			};

			FBO(size_t width, size_t height, ENUM_TYPE type = ENUM_TYPE_BASIC);
			bool PassTo(const FBO & fbo, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;
			bool PassTo(size_t fboID, size_t width, size_t height, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;

			bool Use() const;
			static void UseDefault();
			static size_t DefaultBuffer;
			size_t GetID() const;
			const Texture & GetColorTexture(size_t idx = 0) const;
			const Texture & GetDepthTexture() const;
			bool IsValid() const;
		private:
			bool GenFBO_BASIC(size_t width, size_t height);
			bool GenFBO_RGBF_DEPTH(size_t width, size_t height, size_t colorBufferNum = 1);
			bool GenFBO_MSAA(size_t width, size_t height);
			bool GenFBO_COLOR(size_t width, size_t height, bool isFloat = false);
			bool GenFBO_RED(size_t width, size_t height);
			bool GenFBO_DEPTH(size_t width, size_t height);
			bool GenFBO_CUBE_DEPTH(size_t width, size_t height);
			bool GenFBO_GBUFFER(size_t width, size_t height);
			bool GenFBO_RAYTRACING(size_t width, size_t height);
			bool GenFBO_RTX(size_t width, size_t height);

			bool IsComplete() const;
			static size_t PassType2GL(ENUM_PASS_TYPE passType);

			FBO & operator=(const FBO & fbo) = delete;

			ENUM_TYPE type;
			size_t ID;
			std::vector<Texture> colorTextures;
			Texture depthTexture;
			size_t width;
			size_t height;
			bool isValid;
		};
	}
}
#endif // !_FBO_H_
