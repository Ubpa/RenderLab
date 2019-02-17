#include <CppUtil/OpenGL/FBO.h>

// Choose OpenGL API
#if USE_QT_OPENGL_API
#include <CppUtil/Qt/RawAPI_Define.h>
#else
#include <glad/glad.h>
#endif

#include <cstdio>

using namespace CppUtil::OpenGL;
using namespace std;

#if !USE_QT_OPENGL_API
#define DEFAULT_FRAMEBUFFER 0
#endif

FBO::FBO()
	: type(ENUM_TYPE_INVALID), isValid(false) { }

FBO::FBO(uint width, uint height, ENUM_TYPE type)
	: type(type), width(width), height(height) {
	switch (type)
	{
	case OpenGL::FBO::ENUM_TYPE_BASIC:
		if (!GenFBO_BASIC(width, height))
			printf("GenFBO_BASIC fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_RGBF1_DEPTH:
	case OpenGL::FBO::ENUM_TYPE_RGBF2_DEPTH:
	case OpenGL::FBO::ENUM_TYPE_RGBF3_DEPTH: {
		const int colorBufferNum = type - ENUM_TYPE_RGBF1_DEPTH + 1;
		if (!GenFBO_RGBF_DEPTH(width, height, colorBufferNum))
			printf("GenFBO_RGBF_DEPTH fail\n");
		break;
	}
	case OpenGL::FBO::ENUM_TYPE_MSAA:
		if (!GenFBO_MSAA(width, height))
			printf("GenFBO_MSAA fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_COLOR:
	case OpenGL::FBO::ENUM_TYPE_COLOR_FLOAT: {
		const bool isFloat = type == ENUM_TYPE_COLOR_FLOAT;
		if (!GenFBO_COLOR(width, height, isFloat))
			printf("GenFBO_COLOR fail\n");
		break;
	}
	case OpenGL::FBO::ENUM_TYPE_RED:
		if (!GenFBO_RED(width, height))
			printf("GenFBO_RED fail\n");
		break;
		break;
	case OpenGL::FBO::ENUM_TYPE_DEPTH:
		if (!GenFBO_DEPTH(width, height))
			printf("GenFBO_DEPTH fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_CUBE_DEPTH:
		if (!GenFBO_CUBE_DEPTH(width, height))
			printf("GenFBO_CUBE_DEPTH fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_GBUFFER:
		if (!GenFBO_GBUFFER(width, height))
			printf("GenFBO_GBUFFER fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_RAYTRACING:
		if (!GenFBO_RAYTRACING(width, height))
			printf("GenFBO_RAYTRACING fail\n");
		break;
	case OpenGL::FBO::ENUM_TYPE_RTX:
		if (!GenFBO_RTX(width, height))
			printf("GenFBO_RTX fail\n");
		break;
	default:
		printf("ERROR: FBO type not know\n");
		isValid = false;
		break;
	}
}

bool FBO::GenFBO_BASIC(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	uint colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);

	uint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	return true;
}

bool FBO::GenFBO_RGBF_DEPTH(uint width, uint height, uint colorBufferNum) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	for (uint i = 0; i < colorBufferNum; i++) {
		uint colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID));
	}

	if (colorBufferNum > 1) {
		uint * attachments = new uint[colorBufferNum];
		for (uint i = 0; i < colorBufferNum; i++)
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(colorBufferNum, attachments);
		delete [] attachments;
	}

	uint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		colorTextures.clear();
		return false;
	}

	return true;
}

bool FBO::GenFBO_MSAA(uint width, uint height) {
	const uint samples = 4;
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	uint colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBufferID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBufferID, 0);

	uint RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(Texture(colorBufferID));
	return true;
}

bool FBO::GenFBO_COLOR(uint width, uint height, bool isFloat) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	uint colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, isFloat ? GL_RGB16F : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, isFloat ? GL_RGB16F : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, isFloat ? GL_RGB16F : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, isFloat ? GL_RGB16F : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, isFloat ? GL_RGB16F : GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);	// we only need a color buffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(Texture(colorBufferID));
	return true;
}

bool FBO::GenFBO_RTX(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	uint colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);	// we only need a color buffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(Texture(colorBufferID));
	return true;
}

bool FBO::GenFBO_RED(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	uint colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);	// we only need a color buffer

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(Texture(colorBufferID));
	return true;
}

bool FBO::GenFBO_DEPTH(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	// create depth texture
	uint depthBufferID;
	glGenTextures(1, &depthBufferID);
	glBindTexture(GL_TEXTURE_2D, depthBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBufferID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	depthTexture = Texture(depthBufferID);
	return true;
}

bool FBO::GenFBO_CUBE_DEPTH(uint width, uint height) {
	glGenFramebuffers(1, &ID);

	uint depthBufferID;
	glGenTextures(1, &depthBufferID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthBufferID);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBufferID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	depthTexture = Texture(depthBufferID, Texture::ENUM_TYPE_CUBE_MAP);

	return true;
}

bool FBO::GenFBO_GBUFFER(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	uint gPosition, gNormal, gAlbedoSpec;

	// position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	uint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	uint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	// finally check if framebuffer is complete
	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	colorTextures.push_back(gPosition);
	colorTextures.push_back(gNormal);
	colorTextures.push_back(gAlbedoSpec);

	return true;
}

bool FBO::GenFBO_RAYTRACING(uint width, uint height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	const uint colorBufferNum = 4;
	for (uint i = 0; i < colorBufferNum - 1; i++) {
		uint colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID));
	}
	{
		uint colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID));
	}

	uint attachments[colorBufferNum] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
	};
	glDrawBuffers(colorBufferNum, attachments);

	UseDefault();

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		colorTextures.clear();
		return false;
	}

	return true;
}

uint FBO::GetID() const {
	if (!isValid)
		return 0;
	
	return ID;
}

bool FBO::IsValid() const {
	return isValid;
}

bool FBO::IsComplete() const {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	auto rst = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (rst != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is not complete!\n");
		return false;
	}

	return true;
}

uint FBO::PassType2GL(ENUM_PASS_TYPE passType) {
	switch (passType)
	{
	case OpenGL::FBO::ENUM_PASS_COLOR:
		return GL_COLOR_BUFFER_BIT;
		break;
	case OpenGL::FBO::ENUM_PASS_DEPTH:
		return GL_DEPTH_BUFFER_BIT;
		break;
	default:
		printf("Pass Type is unknown.\n");
		return 0;
		break;
	}
}

bool FBO::PassTo(const FBO & fbo, ENUM_PASS_TYPE passType) const {
	if (!isValid || !fbo.isValid)
		return false;
	
	return PassTo(fbo.ID, fbo.width, fbo.height, passType);
}

bool FBO::PassTo(uint fboID, uint width, uint height, ENUM_PASS_TYPE passType) const {
	if (!isValid)
		return false;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, PassType2GL(passType), GL_NEAREST);
	return true;
}

const Texture & FBO::GetColorTexture(uint idx) const {
	if (!isValid || idx > colorTextures.size())
		return Texture::InValid;

	if (type != ENUM_TYPE_BASIC
		&& type != ENUM_TYPE_MSAA
		&& type != ENUM_TYPE_COLOR
		&& type != ENUM_TYPE_RED
		&& type != ENUM_TYPE_COLOR_FLOAT
		&& type != ENUM_TYPE_RGBF1_DEPTH
		&& type != ENUM_TYPE_RGBF2_DEPTH
		&& type != ENUM_TYPE_RGBF3_DEPTH
		&& type != ENUM_TYPE_GBUFFER
		&& type != ENUM_TYPE_RAYTRACING
		&& type != ENUM_TYPE_RTX)
		return Texture::InValid;

	return colorTextures[idx];
}

const Texture & FBO::GetDepthTexture() const {
	if (!isValid)
		return Texture::InValid;

	if (type != ENUM_TYPE_DEPTH
		&& type != ENUM_TYPE_CUBE_DEPTH)
		return Texture::InValid;

	return depthTexture;
}

bool FBO::Use() const{
	if (!isValid)
		return false;
	
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	return true;
}

void FBO::UseDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER);
}