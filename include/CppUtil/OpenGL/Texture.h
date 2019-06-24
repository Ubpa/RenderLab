#ifndef _OPENGL_TEXTURE_TEXTURE_H_
#define _OPENGL_TEXTURE_TEXTURE_H_

#include <CppUtil/Basic/Ptr.h>

typedef unsigned int uint;

#include <vector>
#include <string>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace OpenGL {
		class Texture {
		public:
			enum ENUM_TYPE
			{
				ENUM_TYPE_NOT_VALID,
				ENUM_TYPE_2D,
				ENUM_TYPE_CUBE_MAP,
				ENUM_TYPE_2D_DYNAMIC,
			};

			Texture();
			Texture(uint ID, ENUM_TYPE type);
			Texture(ENUM_TYPE type);
			Texture(uint width, uint height, const float * data, uint dataType, uint srcFormat, uint internalFormat);
			Texture(const std::vector<std::string> & skybox);
			Texture(const std::vector<Basic::PtrC<Basic::Image>> & skyboxImgs);
			Texture(const std::string & path, bool flip = false, bool gammaCorrection = false);
			Texture(Basic::PtrC<Basic::Image> img);

			bool Load(const std::vector<std::string> & skybox);
			bool Load(const std::string & path, bool flip = false, bool gammaCorrection = false);
			bool SetImg(const Basic::Image & img);
			bool GenBufferForCubemap(uint width, uint height);
			bool GenMipmap();

			bool Use(uint id = 0) const;
			void Bind() const;
			void UnBind() const;

			const uint & GetID() const { return ID; }
			ENUM_TYPE GetType() const { return type; }
			bool IsValid() const;

			void Free();

			static const Texture InValid;
		private:
			static uint Type2GL(ENUM_TYPE type);
			static std::string Type2Str(ENUM_TYPE type);

			uint ID;
			ENUM_TYPE type;
		};
	}
}

#endif // !_OPENGL_TEXTURE_TEXTURE_H_
