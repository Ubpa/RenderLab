#include <OpenGL/Texture.h>

#include <Basic/Image.h>

// Choose OpenGL API
#if USE_QT_OPENGL_API
#include <Qt/RawAPI_Define.h>
#else
#include <glad/glad.h>
#endif

#include <cstdio>

using namespace Ubpa;

using namespace std;

const Texture Texture::InValid(0, ENUM_TYPE_NOT_VALID);

Texture::Texture()
	: ID(0), type(ENUM_TYPE_NOT_VALID) { }

Texture::Texture(unsigned ID, ENUM_TYPE type)
	: ID(ID), type(type) { }

Texture::Texture(ENUM_TYPE type)
	: Texture(0, type) { }

Texture::Texture(PtrC<Image> img) : ID(0), type(ENUM_TYPE_2D_DYNAMIC) {
	SetImg(*img);
}

Texture::Texture(unsigned width, unsigned height, const float * data, unsigned dataType, unsigned srcFormat, unsigned internalFormat, MAG_FILTER magFilter) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, srcFormat, dataType, data);
	//glGenerateMipmap(GL_TEXTURE_2D);
	unsigned glMagFilter = magFilter == MAG_FILTER::NEAREST ? GL_NEAREST : GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	type = ENUM_TYPE_2D;
}

Texture::Texture(const std::string & path, bool flip, bool gammaCorrection){
	type = ENUM_TYPE_NOT_VALID;
	Load(path, flip, gammaCorrection);
}

Texture::Texture(const vector<string> & skybox) {
	type = ENUM_TYPE_NOT_VALID;
	Load(skybox);
}

bool Texture::IsValid() const{
	return ID != 0 && type != ENUM_TYPE_NOT_VALID;
}

bool Texture::Load(const std::vector<std::string> & skybox) {
	if (IsValid()) {
		printf("ERROR: The texture is valid already.\n");
		
		return false;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	for (unsigned i = 0; i < skybox.size(); i++)
	{
		auto img = Image::New(skybox[i]);
		if (!img->IsValid()) {
			printf("Cubemap texture failed to load at path: %s\n", skybox[i].c_str());
			type = ENUM_TYPE_NOT_VALID;
			return false;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img->GetWidth(), img->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, img->GetData());
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	type = ENUM_TYPE_CUBE_MAP;
	UnBind();
	return true;
}

Texture::Texture(const vector<PtrC<Image>> & skyboxImgs) {
	if (skyboxImgs.size() != 6) {
		ID = 0;
		type = ENUM_TYPE_NOT_VALID;
		return;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	for (unsigned i = 0; i < 6; i++)
	{
		if (!skyboxImgs[i]->IsValid()) {
			printf("Cubemap texture load fail\n");
			ID = 0;
			type = ENUM_TYPE_NOT_VALID;
			return;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, skyboxImgs[i]->GetWidth(), skyboxImgs[i]->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, skyboxImgs[i]->GetData());
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	type = ENUM_TYPE_CUBE_MAP;
	UnBind();
}

bool Texture::Load(const std::string & path, bool flip, bool gammaCorrection) {
	if (IsValid()) {
		printf("ERROR: The texture is valid already.\n");
		return false;
	}
	
	auto img = Image::New(path, flip);
	if (!img->IsValid()) {
		printf("ERROR: Texture [%s] load failed\n", path.c_str());
		type = ENUM_TYPE_NOT_VALID;
		return false;
	}

	GLenum internalFormat;
	GLenum dataFormat;
	int nrComponents = img->GetChannel();
	if (nrComponents == 1) {
		internalFormat = GL_RED;
		dataFormat = GL_RED;
	}
	else if (nrComponents == 3) {
		internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
		dataFormat = GL_RGB;
	}
	else if (nrComponents == 4) {
		internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
		dataFormat = GL_RGBA;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	/*
	@1 纹理目标
	@2 多级渐远纹理的级别 （0 为基本级别)
	@3 纹理格式
	@4 width
	@5 height
	@6 0 (历史遗留问题)
	@7 源图格式
	@8 源图类型
	@9 图像数据
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img->GetWidth(), img->GetHeight(), 0, dataFormat, GL_UNSIGNED_BYTE, img->GetData());

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	type = ENUM_TYPE_2D;
	UnBind();
	return true;
}

bool Texture::SetImg(const Image & img) {
	if (type != ENUM_TYPE_2D_DYNAMIC) {
		printf("ERROR: type[%s] can't set image\n", Type2Str(type).c_str());
		return false;
	}

	if (!img.IsValid()) {
		printf("ERROR::Texture::SetImg:\n"
			"\t""img is invalid\n");
		return false;
	}
	
	if (ID == 0)
		glGenTextures(1, &ID);

	GLenum innerFormat, outerFormat;
	int nrComponents = img.GetChannel();
	if (nrComponents == 1) {
		innerFormat = GL_R16F;
		outerFormat = GL_RED;
	}
	else if (nrComponents == 3) {
		innerFormat = GL_RGB16F;
		outerFormat = GL_RGB;
	}
	else if (nrComponents == 4) {
		innerFormat = GL_RGBA16F;
		outerFormat = GL_RGBA;
	}

	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, innerFormat, img.GetWidth(), img.GetHeight(), 0, outerFormat, GL_FLOAT, img.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	UnBind();

	return true;
}

bool Texture::Use(unsigned id) const{
	if (!IsValid()) {
		printf("ERROR::Texture::Use:\n"
			"\t""use a invalid texture\n");
		return false;
	}

	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(Type2GL(type), ID);
	return true;
}

unsigned Texture::Type2GL(ENUM_TYPE type) {
	switch (type)
	{
	case ENUM_TYPE_NOT_VALID:
		return 0;
	case ENUM_TYPE_2D:
	case ENUM_TYPE_2D_DYNAMIC:
		return GL_TEXTURE_2D;
	case ENUM_TYPE_CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;
	default:
		return 0;
	}
}

string Texture::Type2Str(ENUM_TYPE type) {
	switch (type)
	{
	case ENUM_TYPE_NOT_VALID:
		return "ENUM_TYPE_NOT_VALID";
	case ENUM_TYPE_2D:
		return "ENUM_TYPE_2D";
	case ENUM_TYPE_CUBE_MAP:
		return "ENUM_TYPE_CUBE_MAP";
	case ENUM_TYPE_2D_DYNAMIC:
		return "ENUM_TYPE_2D_DYNAMIC";
	default:
		return "UNKNOWN_TYPE";
	}
}

void Texture::Bind() const {
	if (!IsValid())
		return;

	glBindTexture(Type2GL(type), ID);
}

void Texture::UnBind() const{
	if (!IsValid())
		return;
	
	glBindTexture(Type2GL(type), 0);
}

bool Texture::GenBufferForCubemap(unsigned width, unsigned height) {
	if (type != ENUM_TYPE_CUBE_MAP) {
		printf("ERROR::Texture::GenBufferForCubemap:\n"
			"\t""type is not ENUM_TYPE_CUBE_MAP\n");
		return false;
	}

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	for (unsigned i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	type = ENUM_TYPE_CUBE_MAP;
	UnBind();

	return true;
}

bool Texture::GenMipmap() {
	if (!IsValid()) {
		printf("ERROR::Texture::GenMipmap:\n"
			"\t""texture is not valid\n");
		return false;
	}

	Bind();
	auto glType = Type2GL(type);
	glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(glType);
	return true;
}

void Texture::Free() {
	if (!IsValid())
		return;

	glDeleteTextures(1, &ID);
	ID = -1;
	type = ENUM_TYPE_NOT_VALID;
}
