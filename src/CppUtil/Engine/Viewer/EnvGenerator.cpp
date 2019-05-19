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

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

const string rootPath = ROOT_PATH;

namespace CppUtil {
	namespace Engine {
		const string str_PointLight_prefix = "data/shaders/Engine/PointLight/";
		const string str_genDepth = "genDepth";
		const string str_genDepth_vs = str_PointLight_prefix + str_genDepth + ".vs";
		const string str_genDepth_gs = str_PointLight_prefix + str_genDepth + ".gs";
		const string str_genDepth_fs = str_PointLight_prefix + str_genDepth + ".fs";
	}
}

EnvGenerator::EnvGenerator(QT::RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), skyboxSize(1024), irradianceSize(32)
{
	RegMemberFunc<Scene>(&EnvGenerator::Visit);
}

void EnvGenerator::OGL_Init() {
	genSkyboxFBO = FBO(skyboxSize, skyboxSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);
	genIrradianceFBO = FBO(irradianceSize, irradianceSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);

	InitShader_genSkybox();
	InitShader_genIrradiance();
}

void EnvGenerator::InitShader_genSkybox() {
	string vsPath = "data/shaders/Engine/Cubemap/cubemap.vs";
	string fsPath = "data/shaders/Engine/Cubemap/equirectangular_to_cubemap.fs";
	shader_genSkybox = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_genSkybox.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_genSkybox.SetMat4f("projection", captureProjection.GetMatrix().Data());
}

void EnvGenerator::InitShader_genIrradiance() {
	string vsPath = "data/shaders/Engine/Cubemap/cubemap.vs";
	string fsPath = "data/shaders/Engine/Cubemap/irradiance_convolution.fs";
	shader_genIrradiance = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_genIrradiance.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_genIrradiance.SetMat4f("projection", captureProjection.GetMatrix().Data());
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
	if (img == curImg.lock())
		return;

	curImg = img;

	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);

	Transform captureViews[] = {
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
	};

	FBO::TexTarget mapper[6] = {
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_X,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_X,
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Y,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Y,
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Z,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	Texture imgTex(img);

	// gen skybox
	skybox = Texture(Texture::ENUM_TYPE_CUBE_MAP);
	skybox.GenBufferForCubemap(skyboxSize, skyboxSize);

	genSkyboxFBO.Use();
	glViewport(0, 0, skyboxSize, skyboxSize);

	imgTex.Use(0);

	for (int i = 0; i < 6; i++) {
		shader_genSkybox.SetMat4f("view", captureViews[i]);
		genSkyboxFBO.SetColor(skybox, mapper[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOGLW->GetVAO(ShapeType::Cube).Draw(shader_genSkybox);
	}

	// gen irradiance
	irradiance = Texture(Texture::ENUM_TYPE_CUBE_MAP);
	irradiance.GenBufferForCubemap(irradianceSize, irradianceSize);

	genIrradianceFBO.Use();
	glViewport(0, 0, irradianceSize, irradianceSize);

	skybox.Use(0);

	for (int i = 0; i < 6; i++) {
		shader_genIrradiance.SetMat4f("view", captureViews[i]);
		genIrradianceFBO.SetColor(irradiance, mapper[i]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		pOGLW->GetVAO(ShapeType::Cube).Draw(shader_genIrradiance);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);
}

const Texture EnvGenerator::GetSkybox(PtrC<Image> img) const {
	if (curImg.lock() != img) {
		printf("ERROR::EnvGenerator::GetSkybox:\n"
			"\t""img is not regist\n");
	}

	return skybox;
}

const Texture EnvGenerator::GetIrradiance(PtrC<Image> img) const {
	if (curImg.lock() != img) {
		printf("ERROR::EnvGenerator::GetSkybox:\n"
			"\t""img is not regist\n");
	}

	return irradiance;
}

void EnvGenerator::Clear() {
	curImg.reset();
	skybox = Texture::InValid;
	irradiance = Texture::InValid;
}
