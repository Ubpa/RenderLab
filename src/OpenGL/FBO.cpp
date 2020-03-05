#include <OpenGL/FBO.h>

// Choose OpenGL API
#if USE_QT_OPENGL_API
#include <Qt/RawAPI_Define.h>
#else
#include <glad/glad.h>
#endif

#include <cstdio>

using namespace Ubpa;

using namespace std;

#if !USE_QT_OPENGL_API
#define DEFAULT_FRAMEBUFFER 0
#endif

FBO::FBO()
	: type(ENUM_TYPE_INVALID), isValid(false) { }

FBO::FBO(unsigned width, unsigned height, ENUM_TYPE type)
	: type(type), width(width), height(height) {
	switch (type)
	{
	case FBO::ENUM_TYPE_BASIC:
		if (!GenFBO_BASIC(width, height))
			printf("GenFBO_BASIC fail\n");
		break;
	case FBO::ENUM_TYPE_RGBF1_DEPTH:
	case FBO::ENUM_TYPE_RGBF2_DEPTH:
	case FBO::ENUM_TYPE_RGBF3_DEPTH: {
		const int colorBufferNum = type - ENUM_TYPE_RGBF1_DEPTH + 1;
		if (!GenFBO_RGBF_DEPTH(width, height, colorBufferNum))
			printf("GenFBO_RGBF_DEPTH fail\n");
		break;
	}
	case FBO::ENUM_TYPE_MSAA:
		if (!GenFBO_MSAA(width, height))
			printf("GenFBO_MSAA fail\n");
		break;
	case FBO::ENUM_TYPE_COLOR:
	case FBO::ENUM_TYPE_COLOR_FLOAT: {
		const bool isFloat = type == ENUM_TYPE_COLOR_FLOAT;
		if (!GenFBO_COLOR(width, height, isFloat))
			printf("GenFBO_COLOR fail\n");
		break;
	}
	case FBO::ENUM_TYPE_RED:
		if (!GenFBO_RED(width, height))
			printf("GenFBO_RED fail\n");
		break;
		break;
	case FBO::ENUM_TYPE_DEPTH:
		if (!GenFBO_DEPTH(width, height))
			printf("GenFBO_DEPTH fail\n");
		break;
	case FBO::ENUM_TYPE_CUBE_DEPTH:
		if (!GenFBO_CUBE_DEPTH(width, height))
			printf("GenFBO_CUBE_DEPTH fail\n");
		break;
	case FBO::ENUM_TYPE_GBUFFER:
		if (!GenFBO_GBUFFER(width, height))
			printf("GenFBO_GBUFFER fail\n");
		break;
	case FBO::ENUM_TYPE_RAYTRACING:
		if (!GenFBO_RAYTRACING(width, height))
			printf("GenFBO_RAYTRACING fail\n");
		break;
	case FBO::ENUM_TYPE_RTX:
		if (!GenFBO_RTX(width, height))
			printf("GenFBO_RTX fail\n");
		break;
	case FBO::ENUM_TYPE_DYNAMIC_COLOR:
		if (!GenFBO_DYNAMIC_COLOR(width, height))
			printf("GenFBO_DYNAMIC_COLOR fail\n");
		break;
	default:
		printf("ERROR: FBO type not know\n");
		isValid = false;
		break;
	}
}

FBO::FBO(unsigned width, unsigned height, const std::vector<unsigned> & dimVecForGBuffer)
	: width(width), height(height), dimVec(dimVecForGBuffer)
{
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	const unsigned formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
	const unsigned internalFormats[4] = { GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F };
	vector<unsigned> attachments;

	for (int i = 0; i < dimVecForGBuffer.size(); i++) {
		const int dim = dimVecForGBuffer[i];
		unsigned texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormats[dim - 1], width, height, 0, formats[dim - 1], GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texID, 0);

		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		colorTextures.push_back(Texture(texID, Texture::ENUM_TYPE_2D));
	}

	glDrawBuffers(static_cast<GLsizei>(dimVecForGBuffer.size()), attachments.data());

	/*
	// create and attach depth buffer (renderbuffer)
	unsigned rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	*/

	unsigned depthBufferID;
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

	depthTexture = Texture(depthBufferID, Texture::ENUM_TYPE_2D);

	// finally check if framebuffer is complete
	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		type = ENUM_TYPE_INVALID;
		ID = 0;
		return;
	}

	type = ENUM_TYPE_GBUFFER;
	return;
}

bool FBO::GenFBO_BASIC(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	unsigned colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferID, 0);

	unsigned RBO;
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

bool FBO::GenFBO_RGBF_DEPTH(unsigned width, unsigned height, unsigned colorBufferNum) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	for (unsigned i = 0; i < colorBufferNum; i++) {
		unsigned colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	}

	if (colorBufferNum > 1) {
		unsigned * attachments = new unsigned[colorBufferNum];
		for (unsigned i = 0; i < colorBufferNum; i++)
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		glDrawBuffers(colorBufferNum, attachments);
		delete [] attachments;
	}

	unsigned RBO;
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

bool FBO::GenFBO_MSAA(unsigned width, unsigned height) {
	const unsigned samples = 4;
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	unsigned colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorBufferID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorBufferID, 0);

	unsigned RBO;
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

	colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	return true;
}

bool FBO::GenFBO_COLOR(unsigned width, unsigned height, bool isFloat) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	unsigned colorBufferID;
	glGenTextures(1, &colorBufferID);
	glBindTexture(GL_TEXTURE_2D, colorBufferID);
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

	colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	return true;
}

bool FBO::GenFBO_RTX(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	unsigned colorBufferID;
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

	colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	return true;
}

bool FBO::GenFBO_RED(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	// create a color attachment texture
	unsigned colorBufferID;
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

	colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	return true;
}

bool FBO::GenFBO_DEPTH(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	// create depth texture
	unsigned depthBufferID;
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

	depthTexture = Texture(depthBufferID, Texture::ENUM_TYPE_2D);
	return true;
}

bool FBO::GenFBO_CUBE_DEPTH(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);

	unsigned depthBufferID;
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

bool FBO::GenFBO_GBUFFER(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	unsigned gPosition, gNormal, gAlbedoSpec;

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
	unsigned attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned rboDepth;
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

	colorTextures.push_back(Texture(gPosition, Texture::ENUM_TYPE_2D));
	colorTextures.push_back(Texture(gNormal, Texture::ENUM_TYPE_2D));
	colorTextures.push_back(Texture(gAlbedoSpec, Texture::ENUM_TYPE_2D));

	return true;
}

bool FBO::GenFBO_RAYTRACING(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	const unsigned colorBufferNum = 4;
	for (unsigned i = 0; i < colorBufferNum - 1; i++) {
		unsigned colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	}
	{
		unsigned colorBufferID;
		glGenTextures(1, &colorBufferID);
		glBindTexture(GL_TEXTURE_2D, colorBufferID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, colorBufferID, 0);
		colorTextures.push_back(Texture(colorBufferID, Texture::ENUM_TYPE_2D));
	}

	unsigned attachments[colorBufferNum] = {
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

bool FBO::GenFBO_DYNAMIC_COLOR(unsigned width, unsigned height) {
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	// create and attach depth buffer (renderbuffer)
	unsigned rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	UseDefault();

	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		colorTextures.clear();
		return false;
	}

	return true;
}

unsigned FBO::GetID() const {
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

unsigned FBO::PassType2GL(ENUM_PASS_TYPE passType) {
	switch (passType)
	{
	case FBO::ENUM_PASS_COLOR:
		return GL_COLOR_BUFFER_BIT;
		break;
	case FBO::ENUM_PASS_DEPTH:
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

bool FBO::PassTo(unsigned fboID, unsigned width, unsigned height, ENUM_PASS_TYPE passType) const {
	if (!isValid)
		return false;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, PassType2GL(passType), GL_NEAREST);
	return true;
}

const Texture & FBO::GetColorTexture(unsigned idx) const {
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
		&& type != ENUM_TYPE_CUBE_DEPTH
		&& type != ENUM_TYPE_GBUFFER)
		return Texture::InValid;

	return depthTexture;
}

bool FBO::Use() const{
	if (!isValid) {
		printf("ERROR::FBO::Use:\n"
			"\t""FBO is invalid\n");
		return false;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	return true;
}

void FBO::UseDefault() {
	glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FRAMEBUFFER);
}

bool FBO::SetColor(const Texture & tex, TexTarget textarget, int mip) {
	if (!isValid) {
		printf("ERROR::FBO::SetColor:\n"
			"\t""FBO is invalid\n");
		return false;
	}

	GLenum mapper[7] = {
		GL_TEXTURE_2D,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	auto idx = static_cast<unsigned>(textarget);

	if (idx == 0 && (tex.GetType() != Texture::ENUM_TYPE::ENUM_TYPE_2D && tex.GetType() != Texture::ENUM_TYPE::ENUM_TYPE_2D_DYNAMIC)) {
		printf("ERROR::FBO::SetColor:\n"
			"\t""tex is not support GL_TEXTURE_2D\n");
		return false;
	}

	if (idx > 0 && tex.GetType() != Texture::ENUM_TYPE::ENUM_TYPE_CUBE_MAP) {
		printf("ERROR::FBO::SetColor:\n"
			"\t""tex is not support GL_TEXTURE_CUBE_MAP\n");
		return false;
	}

	GLenum glTexTarget = mapper[idx];

	Use();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glTexTarget, tex.GetID(), mip);

	return true;
}

void FBO::Resize(unsigned width, unsigned height) {
	if (!IsValid())
		return;
	if (type != ENUM_TYPE_GBUFFER)
		return;
	if (this->width == width && this->height == height)
		return;

	this->width = width;
	this->height = height;

	// Free all textures
	for (auto colorTex : colorTextures)
		colorTex.Free();
	colorTextures.clear();
	depthTexture.Free();

	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	const unsigned formats[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
	const unsigned internalFormats[4] = { GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F };
	vector<unsigned> attachments;

	for (int i = 0; i < dimVec.size(); i++) {
		const int dim = dimVec[i];
		unsigned texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormats[dim - 1], width, height, 0, formats[dim - 1], GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texID, 0);

		attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		colorTextures.push_back(Texture(texID, Texture::ENUM_TYPE_2D));
	}

	glDrawBuffers(static_cast<GLsizei>(dimVec.size()), attachments.data());

	/*
	// create and attach depth buffer (renderbuffer)
	unsigned rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	*/

	unsigned depthBufferID;
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

	depthTexture = Texture(depthBufferID, Texture::ENUM_TYPE_2D);

	// finally check if framebuffer is complete
	isValid = IsComplete();
	if (!isValid) {
		printf("Framebuffer is not complete!\n");
		type = ENUM_TYPE_INVALID;
		ID = 0;
		return;
	}

	type = ENUM_TYPE_GBUFFER;
	return;
}
