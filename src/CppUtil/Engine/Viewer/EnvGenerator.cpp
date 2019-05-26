#include "EnvGenerator.h"

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>

#include <CppUtil/OpenGL/Camera.h>
#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

const Transform EnvGenerator::captureViews[6] = {
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
	Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
};

const OpenGL::FBO::TexTarget EnvGenerator::mapper[6] = {
	FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_X,
	FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_X,
	FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Y,
	FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Y,
	FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Z,
	FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Z,
};

EnvGenerator::EnvGenerator(QT::RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), skyboxSize(1024), irradianceSize(128), prefilterSize(1024), brdfSize(512)
{
	RegMemberFunc<Scene>(&EnvGenerator::Visit);
}

void EnvGenerator::Init() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	InitFBOs();

	InitShaders();
}

void EnvGenerator::InitFBOs() {
	genSkyboxFBO = FBO(skyboxSize, skyboxSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);
	genIrradianceFBO = FBO(irradianceSize, irradianceSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);

	for (int i = 0; i < maxMipLevels; i++) {
		int curSize = prefilterSize / pow(2, i);
		prefilterFBOs[i] = FBO(curSize, curSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);
	}

	brdfFBO = FBO(brdfSize, brdfSize, FBO::ENUM_TYPE_COLOR_FLOAT);
}

void EnvGenerator::InitShaders() {
	InitShader_genSkybox();
	InitShader_genIrradiance();
	InitShader_Prefilter();
	InitShader_brdf();
}

void EnvGenerator::InitShader_genSkybox() {
	string vsPath = "data/shaders/Engine/IBL/cubemap.vs";
	string fsPath = "data/shaders/Engine/IBL/equirectangular_to_cubemap.fs";
	shader_genSkybox = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_genSkybox.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_genSkybox.SetMat4f("projection", captureProjection);
}

void EnvGenerator::InitShader_genIrradiance() {
	string vsPath = "data/shaders/Engine/IBL/cubemap.vs";
	string fsPath = "data/shaders/Engine/IBL/irradiance_convolution.fs";
	shader_genIrradiance = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_genIrradiance.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_genIrradiance.SetMat4f("projection", captureProjection);
}

void EnvGenerator::InitShader_Prefilter() {
	string vsPath = "data/shaders/Engine/IBL/cubemap.vs";
	string fsPath = "data/shaders/Engine/IBL/prefilter.fs";
	shader_prefilter = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_prefilter.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_prefilter.SetMat4f("projection", captureProjection);
}

void EnvGenerator::InitShader_brdf() {
	string vsPath = Define::str_Basic_P2T2_vs;
	string fsPath = "data/shaders/Engine/IBL/brdf.fs";
	shader_brdf = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);
}

void EnvGenerator::Visit(Ptr<Scene> scene) {
	if (!scene || !scene->GetRoot()) {
		printf("ERROR::EnvGenerator::Visit(Ptr<Scene> scene):\n"
			"\t""scene or scene's root is nullptr\n");
		return;
	}

	auto environment = scene->GetInfiniteAreaLight();
	if (!environment || !environment->GetImg()) {
		Clear();
		return;
	}

	auto img = environment->GetImg();
	UpdateTextures(img);
}

void EnvGenerator::InitBRDF_LUT() {
	if (isInitBrdfFBO)
		return;

	brdfFBO.Use();
	glViewport(0, 0, brdfSize, brdfSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pOGLW->GetVAO(ShapeType::Screen).Draw(shader_brdf);

	isInitBrdfFBO = true;

	// check result
	//
	//auto brdfImg = Image::New(brdfSize, brdfSize, 3);
	//brdfFBO.GetColorTexture(0).Bind();
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, brdfImg->GetData());
	//brdfImg->SaveAsPNG(ROOT_PATH + "data/out/brdf.png", true);
}

void EnvGenerator::UpdateTextures(PtrC<Image> img) {
	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);

	do {
		InitBRDF_LUT();

		if (img == curImg.lock())
			break;

		curImg = img;


		UpdateSkybox();
		UpdateIrradianceMap();
		UpdatePrefilterMap();
	} while (false);

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);
}

void EnvGenerator::UpdateSkybox() {
	skybox = Texture(Texture::ENUM_TYPE_CUBE_MAP);
	skybox.GenBufferForCubemap(skyboxSize, skyboxSize);

	genSkyboxFBO.Use();
	glViewport(0, 0, skyboxSize, skyboxSize);

	Texture imgTex(curImg.lock());
	imgTex.Use(0);

	for (int i = 0; i < 6; i++) {
		shader_genSkybox.SetMat4f("view", captureViews[i]);
		genSkyboxFBO.SetColor(skybox, mapper[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOGLW->GetVAO(ShapeType::Cube).Draw(shader_genSkybox);
	}
	skybox.GenMipmap();
}

void EnvGenerator::UpdateIrradianceMap() {
	irradianceMap = Texture(Texture::ENUM_TYPE_CUBE_MAP);
	irradianceMap.GenBufferForCubemap(irradianceSize, irradianceSize);

	genIrradianceFBO.Use();
	glViewport(0, 0, irradianceSize, irradianceSize);

	skybox.Use(0);

	for (int i = 0; i < 6; i++) {
		shader_genIrradiance.SetMat4f("view", captureViews[i]);
		genIrradianceFBO.SetColor(irradianceMap, mapper[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOGLW->GetVAO(ShapeType::Cube).Draw(shader_genIrradiance);
	}
}

void EnvGenerator::UpdatePrefilterMap() {
	prefilterMap = Texture(Texture::ENUM_TYPE_CUBE_MAP);
	prefilterMap.GenBufferForCubemap(prefilterSize, prefilterSize);
	prefilterMap.GenMipmap();

	for (int mip = 0; mip < maxMipLevels; mip++) {
		prefilterFBOs[mip].Use();

		skybox.Use(0);

		// reisze framebuffer according to mip-level size.
		int curSize = prefilterSize / pow(2, mip);
		glViewport(0, 0, curSize, curSize);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader_prefilter.SetFloat("roughness", roughness);
		for (int i = 0; i < 6; i++) {
			shader_prefilter.SetMat4f("view", captureViews[i]);
			prefilterFBOs[mip].SetColor(prefilterMap, mapper[i], mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			pOGLW->GetVAO(ShapeType::Cube).Draw(shader_prefilter);
		}
	}
}

const Texture EnvGenerator::GetSkybox(PtrC<Image> img) const {
	if (curImg.lock() != img) {
		printf("ERROR::EnvGenerator::GetSkybox:\n"
			"\t""img is not regist\n");
	}

	return skybox;
}

const Texture EnvGenerator::GetIrradianceMap(PtrC<Image> img) const {
	if (curImg.lock() != img) {
		printf("ERROR::EnvGenerator::GetSkybox:\n"
			"\t""img is not regist\n");
	}

	return irradianceMap;
}

const Texture EnvGenerator::GetPrefilterMap(PtrC<Image> img) const {
	if (curImg.lock() != img) {
		printf("ERROR::EnvGenerator::GetSkybox:\n"
			"\t""img is not regist\n");
	}

	return prefilterMap;
}

void EnvGenerator::Clear() {
	curImg.reset();
	skybox = Texture::InValid;
	irradianceMap = Texture::InValid;
	prefilterMap = Texture::InValid;
}
