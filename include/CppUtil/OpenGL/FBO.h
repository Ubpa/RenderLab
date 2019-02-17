#ifndef _OPENGL_FBO_FBO_H_
#define _OPENGL_FBO_FBO_H_

typedef unsigned int uint;

#include <CppUtil/OpenGL/Texture.h>

#include <vector>

namespace CppUtil {
	namespace OpenGL {
		class FBO {
		public:
			enum ENUM_TYPE
			{
				ENUM_TYPE_INVALID,
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

			FBO();
			FBO(uint width, uint height, ENUM_TYPE type = ENUM_TYPE_BASIC);
			bool PassTo(const FBO & fbo, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;
			bool PassTo(uint fboID, uint width, uint height, ENUM_PASS_TYPE passType = ENUM_PASS_COLOR) const;

			bool Use() const;
			static void UseDefault();
			static uint DefaultBuffer;
			uint GetID() const;
			const Texture & GetColorTexture(uint idx = 0) const;
			const Texture & GetDepthTexture() const;
			bool IsValid() const;
		private:
			bool GenFBO_BASIC(uint width, uint height);
			bool GenFBO_RGBF_DEPTH(uint width, uint height, uint colorBufferNum = 1);
			bool GenFBO_MSAA(uint width, uint height);
			bool GenFBO_COLOR(uint width, uint height, bool isFloat = false);
			bool GenFBO_RED(uint width, uint height);
			bool GenFBO_DEPTH(uint width, uint height);
			bool GenFBO_CUBE_DEPTH(uint width, uint height);
			bool GenFBO_GBUFFER(uint width, uint height);
			bool GenFBO_RAYTRACING(uint width, uint height);
			bool GenFBO_RTX(uint width, uint height);

			bool IsComplete() const;
			static uint PassType2GL(ENUM_PASS_TYPE passType);

			ENUM_TYPE type;
			uint ID;
			std::vector<Texture> colorTextures;
			Texture depthTexture;
			uint width;
			uint height;
			bool isValid;
		};
	}
}
#endif // !_OPENGL_FBO_FBO_H_
