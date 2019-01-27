#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <CppUtil/Basic/Ptr.h>

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

			Texture(size_t ID = 0, ENUM_TYPE type = ENUM_TYPE_2D);
			Texture(ENUM_TYPE type);
			Texture(size_t width, size_t height, const float * data, size_t dataType, size_t srcFormat, size_t internalFormat);
			Texture(const std::vector<std::string> & skybox);
			Texture(const std::vector<Basic::CPtr<Basic::Image>> & skyboxImgs);
			Texture(const std::string & path, bool flip = false, bool gammaCorrection = false);

			bool Load(const std::vector<std::string> & skybox);
			bool Load(const std::string & path, bool flip = false, bool gammaCorrection = false);
			bool SetImg(const Basic::Image & img);

			bool Use(size_t id = 0) const;
			void UnBind() const;

			size_t GetID() const;
			bool IsValid() const;

			static const Texture InValid;
		private:
			static size_t Type2GL(ENUM_TYPE type);
			static std::string Type2Str(ENUM_TYPE type);

			size_t ID;
			ENUM_TYPE type;
		};
	}
}

#endif // !_TEXTURE_H_
